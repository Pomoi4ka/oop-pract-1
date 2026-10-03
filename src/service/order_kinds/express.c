#include "../order.h"

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

order_kind *order_kind_express_create(struct context *ctx)
{
    struct express_order_kind *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.vptr = &vtable;
    s->base.name = "Express delivery";
    s->base_price = 500.0f;
    s->per_item = 70.0f;
    return &s->base;
}

static float exp_calc_cost(order_kind *kind, struct order *o)
{
    const struct express_order_kind *s = (const void *)kind;
    return s->base_price + s->per_item * order_get_items_count(o);
}

static float exp_calc_estimated_time(order_kind *kind, struct order *o)
{
    (void) kind;
    (void) o;
    return 12;
}


static int exp_needs_courier(order_kind *kind, struct order *o)
{
    (void) kind;
    (void) o;
    return 1;
}

static enum order_status_transition_result exp_allows_transition(order_kind *kind, struct order *order, enum order_status to)
{
    static const struct allowed_order_status_transition table[] = {
        {ORDER_STATUS_PACKING, ORDER_STATUS_ON_THE_WAY},
        {ORDER_STATUS_ON_THE_WAY, ORDER_STATUS_DELIVERED},
        {-1, -1}
    };

    struct allowed_order_status_transition const *tp = table;

    enum order_status from = order_get_status(order);

    (void) kind;

    for (; (int)tp->from != -1; ++tp) {
        if (tp->from != from) continue;
        if (tp->to != to)     continue;

        switch (from) {
        case ORDER_STATUS_PACKING:
            if (!order_get_courier(order)) return OSTR_COURIER_IS_NOT_SET_YET;
        default:
            break;
        }

        return OSTR_SUCCESS;
    }
    return OSTR_INVALID_NEW_STATUS_FOR_THIS_KIND_OF_ORDER;
}
