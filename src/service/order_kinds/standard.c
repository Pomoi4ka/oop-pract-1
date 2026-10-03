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

order_kind *order_kind_standard_create(struct context *ctx)
{
    struct standard_order_kind *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.vptr = &vtable;
    s->base.name = "Standard delivery";
    s->base_price = 300.0f;
    s->per_item = 50.0f;
    return &s->base;
}

static float std_calc_cost(order_kind *kind, const struct order *o)
{
    const struct standard_order_kind *s = (const void *)kind;
    return s->base_price + s->per_item * order_get_items_count(o);
}

static float std_calc_estimated_time(order_kind *kind, const struct order *o)
{
    (void)o;
    (void)kind;
    return 24;
}

static int std_needs_courier(order_kind *kind, const struct order *o)
{
    (void)o;
    (void)kind;
    return 1;
}

static enum order_status_transition_result
std_allows_transition(order_kind *kind, const struct order *order, enum order_status to)
{
    static const struct allowed_order_status_transition table[] = {
        {ORDER_STATUS_PACKING, ORDER_STATUS_ON_THE_WAY},
        {ORDER_STATUS_ON_THE_WAY, ORDER_STATUS_DELIVERED},
        {-1, -1}
    };

    struct allowed_order_status_transition const *tp;
    enum order_status from;

    (void)kind;

    tp = table;
    from = order_get_status(order);

    for (; (int)tp->from != -1; ++tp) {
        if (tp->from != from) continue;
        if (tp->to != to) continue;

        if (from == ORDER_STATUS_PACKING) {
            if (!order_get_courier(order)) return OSTR_COURIER_IS_NOT_SET_YET;
        }

        return OSTR_SUCCESS;
    }
    return OSTR_INVALID_NEW_STATUS_FOR_THIS_KIND_OF_ORDER;
}
