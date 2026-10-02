#include "../order.h"
#include "../order_kind.h"

#include <assert.h>
#include <stdio.h>

static float exp_calc_cost(struct order *);
static float exp_calc_estim(struct order *);
static void exp_print(struct order_kind *);
static int exp_needs_courier(struct order *);
static int exp_change_status(struct order_kind *, enum order_status);

static const struct order_kind_vtable vtable = {
    exp_calc_cost,
    exp_calc_estim,
    exp_needs_courier,
    exp_print,
    exp_change_status
};

struct express_order_kind {
    struct order_kind base;
    float base_price;
    float per_item;
    struct courier *courier;
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

static float exp_calc_cost(struct order *o)
{
    const struct express_order_kind *s = (const void *)order_get_order_kind(o);
    return s->base_price + s->per_item * order_items_count(o);
}

static float exp_calc_estim(struct order *o)
{
    (void) o;
    return 12;
}


static int exp_needs_courier(struct order *o)
{
    (void) o;
    return 1;
}

static void exp_print(struct order_kind *m)
{
    struct express_order_kind *s = (void *)m;
    (void) s;
    printf("Delivery method: express\n");
}

static int exp_change_status(struct order_kind *kind, enum order_status status)
{
    (void) kind;
    (void) status;
    assert(0);
}
