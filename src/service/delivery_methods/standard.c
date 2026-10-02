#include "../delivery_method.h"

static float std_calc_cost(struct delivery_method *, struct order *);
static float std_calc_estim(struct delivery_method *, struct order *);
static int std_assign_courier(struct delivery_method *, struct courier *);

static struct allowed_order_status_transition std_trans_table[] = {
    {ORDER_STATUS_PACKING, ORDER_STATUS_ON_THE_WAY},
    {ORDER_STATUS_ON_THE_WAY, ORDER_STATUS_DELIVERED},
    {-1, -1}
};

static const struct delivery_method_vtable vtable = {
    "standard delivery",
    std_trans_table,
    1, /* requires_courier */
    std_calc_cost,
    std_calc_estim,
    std_assign_courier,
};

struct standard_delivery_method {
    struct delivery_method base;
    float base_price;
    float per_item;
    struct courier *courier;
};

struct delivery_method *standard_delivery_method_create(struct context *ctx)
{
    struct standard_delivery_method *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.vptr = &vtable;
    s->base_price = 300.0f;
    s->per_item = 50.0f;
    return &s->base;
}

static float std_calc_cost(struct delivery_method *m, struct order *o)
{
    const struct standard_delivery_method *s = (const void *)m;
    return s->base_price + s->per_item * order_items_count(o);
}

static float std_calc_estim(struct delivery_method *dm, struct order *o)
{
    (void) dm;
    (void) o;
    return 24;
}


static int std_assign_courier(struct delivery_method *m, struct courier *c)
{
    struct standard_delivery_method *s = (void *)m;
    if (s->courier) return 0;
    s->courier = c;
    return 1;
}
