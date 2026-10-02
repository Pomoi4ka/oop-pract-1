#ifndef COURIER_H_
#define COURIER_H_

#include "../runtime/context.h"

enum {
    COURIER_NOTES_NONE    = 0x0,
    COURIER_NOTES_HAS_CAR = 0x1
};

struct courier;

struct courier *courier_create(struct context *, const char *name, int);

#endif /* COURIER_H_ */
