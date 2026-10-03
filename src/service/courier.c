#include "courier.h"

#include <stdio.h>
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

void courier_print(courier *c)
{
    (void)c;
    assert(0 && "todo");
}
