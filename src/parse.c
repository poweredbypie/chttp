#include "parse.h"

#include <string.h>

HttpRequest parseRequest(const Slice* buf) {
    HttpRequest req;

    char* verb_end = strstr(buf->buf, " ");
    Slice verb = sliceFrom(buf->buf, verb_end - buf->buf + 1);
    verb.buf[verb.size - 1] = 0;

    if (strstr(verb.buf, "GET")) {
        req.verb = GET;
    }
    else if (strstr(verb.buf, "POST")) {
        req.verb = POST;
    }
    else {
        die("Unimplemented request type", 1);
    }
    free(verb.buf);

    char* path_start = verb_end + 1;
    req.path = sliceFrom(path_start, strstr(path_start, " ") - path_start + 1);
    req.path.buf[req.path.size - 1] = 0;

    return req;
}

void freeRequest(HttpRequest* req) {
    free(req->path.buf);
}
