#ifndef ROUTER_H
#define ROUTER_H

#include "parse.h"
#include "shared.h"

#include <stdint.h>

Slice routerGetPage(Slice path, HttpStatus status);

#endif