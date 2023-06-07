#define _CRT_SECURE_NO_WARNINGS
#include "router.h"
#include "shared.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

const char template[] = "\
HTTP/1.0 200 OK\n\
Server: My Server Is Better Than BlobKat's v0.0.1\n\
";

const Slice root = mkslice("root/");

Slice getFileContents(FILE* file) {
    const int old = ftell(file);
    fseek(file, 0, SEEK_END);
    const int length = ftell(file);
    fseek(file, old, SEEK_SET);

    char* buf = malloc(length + 1);
    fread(buf, 1, length, file);
    buf[length] = 0;

    Slice contents = {
        .buf = buf,
        .size = length
    };
    return contents;
}

Slice routerGetPage(Slice path) {
    // Copy over the template
    Slice buf = {
        .buf = malloc(sizeof(template)),
        .size = sizeof(template)
    };
    memcpy(buf.buf, template, buf.size - 1);
    buf.buf[buf.size - 1] = 0;

    // Add root/ to the front of the path
    Slice filepath = {
        .buf = NULL,
        .size = root.size
    };
    filepath.buf = malloc(filepath.size);
    memcpy(filepath.buf, root.buf, root.size);
    filepath.buf[root.size - 1] = 0;
    slice_append(&filepath, &path);

    // Actually open the file
    FILE* file = fopen(filepath.buf, "r");

    if (file == NULL) {
        Slice ret = {
            .buf = NULL,
            .size = 0
        };
        return ret;
    }

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

    Slice filetype = slice_sprintf("Content-Type: %s; charset=utf-8\n", filetype_str);
    printf("%s", filetype.buf);
    filetype.size--;
    slice_append(&buf, &filetype);
    free(filetype.buf);

    Slice size = slice_sprintf("Content-Length: %llu\n\n", contents.size);
    size.size--;
    slice_append(&buf, &size);
    free(size.buf);

    slice_append(&buf, &contents);
    buf.buf[buf.size - 1] = 0;
    free(contents.buf);

    return buf;
}