#ifndef DELIVERY_METHOD_H_
#define DELIVERY_METHOD_H_

#include "order.h"

struct delivery_method;
struct order;

struct delivery_method_vtable {
    const char *name;
    struct allowed_order_status_transition *order_status_transition_table;
    int requires_courier;
    float (*calc_cost)(struct delivery_method *, struct order *);
    float (*calc_estimated_time)(struct delivery_method *, struct order *);
    int (*assign_courier)(struct delivery_method *, struct courier *);
};

struct delivery_method {
    const struct delivery_method_vtable *vptr;
};

#endif /* DELIVERY_METHOD_H_ */
