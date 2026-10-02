#include "../order_kind.h"
#include "../order.h"

#include <assert.h>

#define M(ret, name, pnames, ptypes) static ret std_##name ptypes;
ORDER_KIND_METHODS
#undef M

static const struct order_kind_vtable vtable = {
#define M(ret, name, pnames, ptypes) std_##name,
ORDER_KIND_METHODS
#undef M
};

struct standard_order_kind {
    struct order_kind base;
    float base_price;
    float per_item;
};

struct order_kind *standard_order_kind_create(struct context *ctx)
{
    struct standard_order_kind *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.vptr = &vtable;
    s->base_price = 300.0f;
    s->per_item = 50.0f;
    return &s->base;
}

static float std_calc_cost(struct order_kind *kind, struct order *o)
{
    const struct standard_order_kind *s = (const void *)kind;
    return s->base_price + s->per_item * order_get_items_count(o);
}

static float std_calc_estimated_time(struct order_kind *kind, struct order *o)
{
    (void) o;
    (void) kind;
    return 24;
}


static int std_needs_courier(struct order_kind *kind, struct order *o)
{
    (void)o;
    (void)kind;
    return 1;
}

static void std_print(struct order_kind *kind)
{
    (void) kind;
    assert(0);
}

static int std_allows_transition(struct order_kind *kind, enum order_status status)
{
    (void) kind;
    (void) status;
    assert(0);
}
