#include "router.h"

#include "shared.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const Slice template = mkslice("HTTP/1.1 ");

const Slice root = mkslice("root/");

Slice getFileContents(FILE* file) {
    Slice buf;
    const int old = ftell(file);
    fseek(file, 0, SEEK_END);
    buf.size = ftell(file);
    fseek(file, old, SEEK_SET);

    buf.buf = alloc(char, buf.size + 1);
    fread(buf.buf, 1, buf.size, file);
    buf.buf[buf.size] = 0;

    return buf;
}

Slice routerGetPage(Slice path, HttpStatus status) {
    // Copy over the template
    Slice buf = sliceClone(&template);

    // Add root/ to the front of the path
    Slice filepath = sliceClone(&root);
    sliceAdd(&filepath, &path);

    // Actually open the file
    FILE* file = fopen(filepath.buf, "r");

    if (file == NULL) {
        Slice ret = { .buf = NULL, .size = 0 };
        return ret;
    }

    Slice status_str;
    switch (status) {
    case 200:
        status_str = mkslice("200 OK\n");
        break;
    case 404:
        status_str = mkslice("404 Not Found\n");
        break;
    default:
        status_str = mkslice("418 I'm a teapot\n");
        break;
    }
    status_str.size--;
    sliceAdd(&buf, &status_str);

    Slice server_str = mkslice("Server: My Server Is Better Than BlobKat's v0.0.1\n");
    server_str.size--;
    sliceAdd(&buf, &server_str);

    Slice contents = getFileContents(file);
    fclose(file);

    char* filetype_str = "text/plain";

    char* ext = strstr(filepath.buf, ".");
    if (strstr(ext, "html")) {
        filetype_str = "text/html";
    }
    else if (strstr(ext, "js")) {
        filetype_str = "text/javascript";
    }
    free(filepath.buf);

    Slice filetype = sliceFmt("Content-Type: %s; charset=utf-8\n", filetype_str);
    printf("%s", filetype.buf);
    filetype.size--;
    sliceAdd(&buf, &filetype);
    free(filetype.buf);

    Slice size = sliceFmt("Content-Length: %llu\n\n", contents.size);
    size.size--;
    sliceAdd(&buf, &size);
    free(size.buf);

    sliceAdd(&buf, &contents);
    buf.buf[buf.size - 1] = 0;
    free(contents.buf);

    return buf;
}