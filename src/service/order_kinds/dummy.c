#include "../order_kind.h"

#include <stdio.h>

static void dummy_print(struct order_kind *kind);

static const struct order_kind_vtable vtable = {
    NULL,
    NULL,
    NULL,
    dummy_print,
    NULL,
};

struct dummy_order_kind {
    struct order_kind base;
};

struct order_kind *order_kind_dummy_create(struct context *ctx)
{
    struct dummy_order_kind *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.vptr = &vtable;
    return &s->base;
}

static void dummy_print(struct order_kind *kind)
{
    (void) kind;
}
