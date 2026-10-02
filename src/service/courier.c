#include "courier.h"

struct courier {
    const char *name;
    int notes;
};

struct courier *courier_create(struct context *ctx, const char *name, int notes)
{
    struct courier *c = context_alloc(ctx, sizeof *c);
    c->name = name;
    c->notes = notes;
    return c;
}

int courier_has_car(struct courier *c)
{
    return !!(c->notes & COURIER_NOTES_HAS_CAR);
}
