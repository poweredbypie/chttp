#ifndef SLICE_H
#define SLICE_H

typedef struct {
    char* buf;
    size_t size;
} Slice;

void sliceAdd(Slice* dest, const Slice* src);
Slice sliceFmt(const char* fmt, ...);
Slice sliceFrom(const char* str, size_t size);
Slice sliceClone(const Slice* slice);

#define mkslice(str) \
    (Slice) { .buf = str, .size = sizeof(str) }

#endif