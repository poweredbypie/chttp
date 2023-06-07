#ifndef SHARED_H
#define SHARED_H

#include "slice.h"

#include <stdio.h>
#include <stdlib.h>

inline void die(const char* err, int code) {
    fprintf(stderr, "%s", err);
    exit(code);
}

#define alloc(type, size) (type*)malloc(size)
#define morph(type, buf, size) (type*)realloc(buf, size)

#endif