#ifndef SHARED_H
#define SHARED_H

#include <stdio.h>
#include <stdlib.h>

void die(const char* err, int code);

typedef struct {
    char* buf;
    size_t size;
} Slice;

Slice slice_sprintf(const char* fmt, ...);

void slice_append(Slice* dest, const Slice* src);

void slice_from(const char* str);

#define mkslice(str) \
(Slice){ \
    .buf = str, \
    .size = sizeof(str) \
}

#endif