#include "../delivery_method.h"

static float exp_calc_cost(struct delivery_method *, struct order *);
static float exp_calc_estim(struct delivery_method *, struct order *);
static int exp_assign_courier(struct delivery_method *, struct courier *);

static struct allowed_order_status_transition exp_trans_table[] = {
    {ORDER_STATUS_PACKING, ORDER_STATUS_ON_THE_WAY},
    {ORDER_STATUS_ON_THE_WAY, ORDER_STATUS_DELIVERED},
    {-1, -1}
};

static const struct delivery_method_vtable vtable = {
    "express delivery",
    exp_trans_table,
    1, /* requires_courier */
    exp_calc_cost,
    exp_calc_estim,
    exp_assign_courier,
};

struct standard_delivery_method {
    struct delivery_method base;
    float base_price;
    float per_item;
    struct courier *courier;
};

struct delivery_method *express_delivery_method_create(struct context *ctx)
{
    struct standard_delivery_method *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.vptr = &vtable;
    s->base_price = 500.0f;
    s->per_item = 70.0f;
    return &s->base;
}

static float exp_calc_cost(struct delivery_method *m, struct order *o)
{
    const struct standard_delivery_method *s = (const void *)m;
    return s->base_price + s->per_item * order_items_count(o);
}

static float exp_calc_estim(struct delivery_method *dm, struct order *o)
{
    (void) dm;
    (void) o;
    return 12;
}


static int exp_assign_courier(struct delivery_method *m, struct courier *c)
{
    struct standard_delivery_method *s = (void *)m;
    if (s->courier) return 0;
    s->courier = c;
    return 1;
}
