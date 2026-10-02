#include "../order.h"
#include "../order_kind.h"

#include <assert.h>
#include <stdio.h>

#define M(ret, name, pnames, ptypes) static ret exp_##name ptypes;
ORDER_KIND_METHODS
#undef M

static const struct order_kind_vtable vtable = {
#define M(ret, name, pnames, ptypes) exp_##name,
ORDER_KIND_METHODS
#undef M
};

struct express_order_kind {
    struct order_kind base;
    float base_price;
    float per_item;
};

struct order_kind *express_order_kind_create(struct context *ctx)
{
    struct express_order_kind *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.vptr = &vtable;
    s->base_price = 500.0f;
    s->per_item = 70.0f;
    return &s->base;
}

static float exp_calc_cost(struct order_kind *kind, struct order *o)
{
    const struct express_order_kind *s = (const void *)kind;
    return s->base_price + s->per_item * order_get_items_count(o);
}

static float exp_calc_estimated_time(struct order_kind *kind, struct order *o)
{
    (void) kind;
    (void) o;
    return 12;
}


static int exp_needs_courier(struct order_kind *kind, struct order *o)
{
    (void) kind;
    (void) o;
    return 1;
}

static void exp_print(struct order_kind *m)
{
    struct express_order_kind *s = (void *)m;
    (void) s;
    printf("Delivery method: express\n");
}

static int exp_allows_transition(struct order_kind *kind, enum order_status status)
{
    (void) kind;
    (void) status;
    assert(0);
}
