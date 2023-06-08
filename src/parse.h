#ifndef PARSE_H
#define PARSE_H

#include "shared.h"

#include <stdint.h>

typedef enum {
    GET,
    HEAD,
    POST,
    PUT,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH
} HttpVerb;

typedef uint16_t HttpStatus;

typedef struct {
    HttpVerb verb;
    Slice path;
} HttpRequest;

HttpRequest parseRequest(const Slice* buf);
void freeRequest(HttpRequest* req);

#endif