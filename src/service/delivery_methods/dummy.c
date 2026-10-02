#include "../delivery_method.h"

static struct allowed_order_status_transition trans_table[] = {
    {-1, -1}
};

static const struct delivery_method_vtable vtable = {
    "unset delivery",
    trans_table,
    0, /* requires_courier */
    NULL,
    NULL,
    NULL,
};

struct standard_delivery_method {
    struct delivery_method base;
};

struct delivery_method *dummy_delivery_method_create(struct context *ctx)
{
    struct standard_delivery_method *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.vptr = &vtable;
    return &s->base;
}
