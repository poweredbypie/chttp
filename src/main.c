#include <WinSock2.h>
#include <WS2tcpip.h>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "router.h"
#include "parse.h"
#include "shared.h"

void initWinSock() {
    WSADATA data;
    int initErr = WSAStartup(MAKEWORD(2, 2), &data);
    if (initErr) {
        die("Failed to init WinSock lib", initErr);
    }
}

typedef struct addrinfo AddrInfo;
AddrInfo* getAddrInfo(const char* port) {
    AddrInfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP,
        .ai_flags = AI_PASSIVE
    };

    AddrInfo* res;

    int addrErr = getaddrinfo(NULL, port, &hints, &res);
    if (addrErr) {
        WSACleanup();
        die("Failed to get socket info", addrErr);
    }

    return res;
}

typedef SOCKET Socket;
Socket initSocket(AddrInfo* info) {
    Socket sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    if (sock == INVALID_SOCKET) {
        freeaddrinfo(info);
        WSACleanup();
        die("Failed to get a valid socket", WSAGetLastError());
    }

    int bindErr = bind(sock, info->ai_addr, info->ai_addrlen);
    if (bindErr) {
        freeaddrinfo(info);
        closesocket(sock);
        WSACleanup();
        die("Couldn't bind socket", WSAGetLastError());
    }

    freeaddrinfo(info);

    if (listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(sock);
        WSACleanup();
        die("Couldn't start listening on socket", WSAGetLastError());
    }

    return sock;
}

void handleRequest(Socket request) {
    int recvLen = 512;
    Slice buf = {
        .buf = NULL,
        .size = 0
    };
    while (1) {
        buf.buf = morph(char, buf.buf, recvLen);
        buf.size = recvLen;

        int recvLen = 0;
        recvLen = recv(request, buf.buf, buf.size, 0);
        if (recvLen == WSAEMSGSIZE) {
            recvLen += 512;
            continue;
        }
        if (recvLen > 0) {
            HttpRequest req = parseRequest(&buf);
            printf("Path %s (type %d)\n", req.path.buf, req.verb);

            Slice page;
            // Special case for root - get index.html
            if (strcmp(req.path.buf, "/") == 0) {
                page = routerGetPage(mkslice("index.html"));
            }
            else {
                page = routerGetPage(req.path);
            }

            // If we couldn't find the page,
            if (page.buf == NULL) {
                page = routerGetPage(mkslice("404.html"));
            }

            int sendLen = send(request, page.buf, page.size - 1, 0);
            free(page.buf);
            if (sendLen == SOCKET_ERROR) {
                closesocket(request);
                WSACleanup();
                die("Couldn't send response", WSAGetLastError());
            }
            printf("Sent %d bytes\n", sendLen);
            break;
        }
        else if (recvLen < 0) {
            closesocket(request);
            WSACleanup();
            die("Couldn't receive request", WSAGetLastError());
        }
    }
    int stopErr = shutdown(request, SD_SEND);
    if (stopErr) {
        closesocket(request);
        WSACleanup();
        die("Couldn't finalize request", WSAGetLastError());
    }
    printf("Finished handling request\n");
    free(buf.buf);
}

void handleRequests(Socket sock) {
    while (1) {
        Socket clientSocket = accept(sock, NULL, NULL);
        printf("Received request\n");
        if (clientSocket == INVALID_SOCKET) {
            closesocket(sock);
            WSACleanup();
            die("Couldn't accept request", WSAGetLastError());
        }
        handleRequest(clientSocket);
        closesocket(clientSocket);
    }
}

int main() {
    initWinSock();
    AddrInfo* info = getAddrInfo("8080");
    Socket sock = initSocket(info);
    handleRequests(sock);
}