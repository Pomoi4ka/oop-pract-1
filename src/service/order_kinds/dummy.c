#include "../order_kind.h"

#include <stdio.h>

#define M(ret, name, pnames, ptypes) static ret dummy_##name ptypes;
ORDER_KIND_METHODS
#undef M

static const struct order_kind_vtable vtable = {
#define M(ret, name, pnames, ptypes) dummy_##name,
ORDER_KIND_METHODS
#undef M
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

static int dummy_needs_courier(struct order_kind *kind, struct order *ord)
{
    (void) kind;
    (void) ord;
    return 0;
}

static float dummy_calc_cost(struct order_kind *kind, struct order *ord)
{
    (void) kind;
    (void) ord;
    return 0;
}

static float dummy_calc_estimated_time(struct order_kind *kind, struct order *ord)
{
    (void) kind;
    (void) ord;
    return 0;
}

static int dummy_allows_transition(struct order_kind *kind, enum order_status status)
{
    (void) kind;
    (void) status;
    return 0;
}

static void dummy_print(struct order_kind *kind)
{
    (void) kind;
}
