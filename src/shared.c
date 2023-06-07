#include "shared.h"

#include <string.h>
#include <stdarg.h>

void die(const char* err, int code) {
    fprintf(stderr, "%s", err);
    exit(code);
}

void slice_append(Slice* dest, const Slice* src) {
    dest->buf = realloc(dest->buf, dest->size + src->size);
    memcpy(dest->buf + dest->size - 1, src->buf, src->size);
    dest->size += src->size;
}

void slice_from(const char* str) {
    
}

Slice slice_sprintf(const char* fmt, ...) {
    Slice buf;
    va_list list;
    va_start(list, fmt);
    buf.size = vsnprintf(NULL, 0, fmt, list) + 1;
    buf.buf = malloc(buf.size);
    vsnprintf(buf.buf, buf.size, fmt, list);
    va_end(list);
    return buf;
}