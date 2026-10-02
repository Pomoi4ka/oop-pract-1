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

struct order_kind_vtable {
    float (*calc_cost)(struct order *);
    float (*calc_estimated_time)(struct order *);
    int (*needs_courier)(struct order *);
    void (*print)(struct order_kind *);
    int (*change_status)(struct order_kind *, enum order_status);
};

struct order_kind {
    const struct order_kind_vtable *vptr;
};

struct order_kind *order_kind_standard_create(struct context *ctx);
struct order_kind *order_kind_express_create(struct context *ctx);
struct order_kind *order_kind_dummy_create(struct context *ctx);

#endif /* ORDER_KIND_H_ */
