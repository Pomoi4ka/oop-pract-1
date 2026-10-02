#include "../order_kind.h"
#include "../order.h"

#include <assert.h>

static float std_calc_cost(struct order *);
static float std_calc_estim(struct order *);
static int std_needs_courier(struct order *);
static void std_print(struct order_kind *);
static int std_change_status(struct order_kind *, enum order_status);

static const struct order_kind_vtable vtable = {
    std_calc_cost,
    std_calc_estim,
    std_needs_courier,
    std_print,
    std_change_status,
};

struct standard_order_kind {
    struct order_kind base;
    float base_price;
    float per_item;
    struct courier *courier;
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

static float std_calc_cost(struct order *o)
{
    const struct standard_order_kind *s = (const void *)order_get_order_kind(o);
    return s->base_price + s->per_item * order_items_count(o);
}

static float std_calc_estim(struct order *o)
{
    (void) o;
    return 24;
}


static int std_needs_courier(struct order *o)
{
    (void)o;
    return 1;
}

static void std_print(struct order_kind *kind)
{
    (void) kind;
    assert(0);
}

static int std_change_status(struct order_kind *kind, enum order_status status)
{
    (void) kind;
    (void) status;
    assert(0);
}
