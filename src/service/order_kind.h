#ifndef ORDER_KIND_H_
#define ORDER_KIND_H_

struct order_kind;
struct order;

#include "order.h"

enum order_status {
    ORDER_STATUS_PACKING,
    ORDER_STATUS_READY_FOR_PICKUP,
    ORDER_STATUS_ON_THE_WAY,
    ORDER_STATUS_DELIVERED,
    ORDER_STATUS_PICKED
};

struct allowed_order_status_transition {
    enum order_status from, to;
};

typedef const struct order_kind order_kind;

#define ORDER_KIND_METHODS \
    M(float, calc_cost, (kind, order), (order_kind *, struct order *)) \
    M(float, calc_estimated_time, (kind, order), (order_kind *, struct order *)) \
    M(int, needs_courier, (kind, order), (order_kind *, struct order *)) \
    M(enum order_status_transition_result, allows_transition, (kind, order, status), (order_kind *, struct order *, enum order_status)) \

struct order_kind_vtable {
#define M(ret, name, pnames, ptypes) ret (*name)ptypes;
    ORDER_KIND_METHODS
#undef M
};

struct order_kind {
    const struct order_kind_vtable *vptr;
    const char *name;
};

order_kind *order_kind_standard_create(struct context *ctx);
order_kind *order_kind_express_create(struct context *ctx);

#endif /* ORDER_KIND_H_ */
