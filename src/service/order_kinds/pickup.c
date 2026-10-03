#include "../order.h"

#include <assert.h>

#define M(ret, name, pnames, ptypes) static ret pkp_##name ptypes;
ORDER_KIND_METHODS
#undef M

static const struct order_kind_vtable vtable = {
#define M(ret, name, pnames, ptypes) pkp_##name,
ORDER_KIND_METHODS
#undef M
};

struct pickup_order_kind {
    struct order_kind base;
    float base_price;
    float per_item;
};

order_kind *order_kind_pickup_create(struct context *ctx)
{
    struct pickup_order_kind *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.vptr = &vtable;
    s->base.name = "Self pickup";
    s->base_price = 0.0f;
    s->per_item = 0.0f;
    return &s->base;
}

static float pkp_calc_cost(order_kind *kind, struct order *o)
{
    const struct pickup_order_kind *s = (const void *)kind;
    return s->base_price + s->per_item * order_get_items_count(o);
}

static float pkp_calc_estimated_time(order_kind *kind, struct order *o)
{
    (void)kind;
    (void)o;
    return 1;
}

static int pkp_needs_courier(order_kind *kind, struct order *o)
{
    (void)kind;
    (void)o;
    return 0;
}

static enum order_status_transition_result
pkp_allows_transition(order_kind *kind, struct order *order, enum order_status to)
{
    static const struct allowed_order_status_transition table[] = {
        {ORDER_STATUS_PACKING,         ORDER_STATUS_READY_FOR_PICKUP},
        {ORDER_STATUS_READY_FOR_PICKUP, ORDER_STATUS_PICKED},
        {-1, -1}
    };

    struct allowed_order_status_transition const *tp;
    enum order_status from;

    (void)kind;

    from = order_get_status(order);

    for (tp = table; (int)tp->from != -1; ++tp) {
        if (tp->from != from) continue;
        if (tp->to   != to)   continue;
        return OSTR_SUCCESS;
    }
    return OSTR_INVALID_NEW_STATUS_FOR_THIS_KIND_OF_ORDER;
}
