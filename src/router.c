#include "router.h"

#include "shared.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const Slice template = mkslice(
    "\
HTTP/1.0 200 OK\n\
Server: My Server Is Better Than BlobKat's v0.0.1\n\
"
);

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

Slice routerGetPage(Slice path) {
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