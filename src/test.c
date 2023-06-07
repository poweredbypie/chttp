#include <stdio.h>
#include <stdlib.h>

#include "router.h"

int main() {
    Slice root = routerGetPage(mkslice("index.html"));
    printf("index.html:\n%s\n", root.buf);
    free(root.buf);

    Slice not_found = routerGetPage(mkslice("404.html"));
    printf("404.html:\n%s\n", not_found.buf);
    free(not_found.buf);
}