#include "slice.h"

#include "shared.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Slice sliceFrom(const char* str, size_t size) {
    Slice ret = { .buf = alloc(char, size), .size = size };
    memcpy(ret.buf, str, size);
    return ret;
}

Slice sliceFmt(const char* fmt, ...) {
    Slice buf;
    va_list list;
    va_start(list, fmt);
    buf.size = vsnprintf(NULL, 0, fmt, list) + 1;
    buf.buf = alloc(char, buf.size);
    vsnprintf(buf.buf, buf.size, fmt, list);
    va_end(list);
    return buf;
}

Slice sliceClone(const Slice* slice) {
    Slice clone = { .buf = alloc(char, slice->size), .size = slice->size };
    memcpy(clone.buf, slice->buf, slice->size);
    return clone;
}

void sliceAdd(Slice* dest, const Slice* src) {
    dest->buf = morph(char, dest->buf, dest->size + src->size);
    memcpy(dest->buf + dest->size - 1, src->buf, src->size);
    dest->size += src->size;
}