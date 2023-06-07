#include "parse.h"

#include <string.h>

HttpRequest parseRequest(char* buf) {
    HttpRequest req;
    
    char* verb_end = strstr(buf, " ");
    Slice verb = {
        .buf = NULL,
        .size = verb_end - buf
    };

    verb.buf = malloc(verb.size + 1);
    memcpy(verb.buf, buf, verb.size);
    verb.buf[verb.size] = 0;
    if (strstr(verb.buf, "GET")) {
        req.verb = GET;
    } else if (strstr(verb.buf, "POST")) {
        req.verb = POST;
    }
    else {
        die("Unimplemented request type", 1);
    }
    free(verb.buf);

    char* path_start = verb_end + 1;
    Slice path = {
        .buf = NULL,
        .size = strstr(path_start, " ") - path_start + 1
    };
    path.buf = malloc(path.size);
    memcpy(path.buf, path_start, path.size - 1);
    path.buf[path.size - 1] = 0;
    req.path = path;

    return req;
}

void freeRequest(HttpRequest* req) {
    free(req->path.buf);
}
