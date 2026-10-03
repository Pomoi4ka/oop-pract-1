#include "courier.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

struct courier {
    const char *name;
    int notes;
};

courier *courier_create(struct context *ctx, const char *name, int notes)
{
    struct courier *c = context_alloc(ctx, sizeof *c);
    c->name = name;
    c->notes = notes;
    return c;
}

void courier_print(courier *c, int pad)
{
    printf("%*sCourier name: %s\n", pad, "", c->name);
    printf("%*sCourier notes:\n", pad, "");
    if (c->notes & COURIER_NOTES_HAS_CAR)
        printf("%*shas car\n", pad+2, "");
}

unsigned courier_hasheq(enum hasheq_op op, const void *pa, const void *pb)
{
    courier *const *a, *const *b;
    a = pa;
    b = pb;
    switch (op) {
    case HASHEQ_EQ:
        return strcmp((*a)->name, (*b)->name) == 0;
    case HASHEQ_HASH:
        return fnv1((*a)->name, strlen((*a)->name));
    }
    return -1;
}
