#ifndef ORDER_KIND_H_
#define ORDER_KIND_H_

struct order_kind;
struct order;

#include "courier.h"

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

#define ORDER_KIND_METHODS \
    M(float, calc_cost, (kind, order), (struct order_kind *, struct order *)) \
    M(float, calc_estimated_time, (kind, order), (struct order_kind *, struct order *)) \
    M(int, needs_courier, (kind, order), (struct order_kind *, struct order *)) \
    M(void, print, (kind), (struct order_kind *)) \
    M(int, allows_transition, (kind, status), (struct order_kind *, enum order_status)) \

struct order_kind_vtable {
#define M(ret, name, pnames, ptypes) ret (*name)ptypes;
    ORDER_KIND_METHODS
#undef M
};

struct order_kind {
    const struct order_kind_vtable *vptr;
};

struct order_kind *order_kind_standard_create(struct context *ctx);
struct order_kind *order_kind_express_create(struct context *ctx);
struct order_kind *order_kind_dummy_create(struct context *ctx);

#endif /* ORDER_KIND_H_ */
