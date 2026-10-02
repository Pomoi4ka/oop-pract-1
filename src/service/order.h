#ifndef ORDER_H_
#define ORDER_H_

#include "../runtime/context.h"
#include "courier.h"
#include "client.h"
#include "delivery_method.h"

struct delivery_method;

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

struct order;

struct order *order_create(struct context *, int id, struct client_data *);
int order_get_id(struct order const *);
void order_print(const struct order *);
void order_add_item(struct order *o, const char *name, int q, float price);
size_t order_items_count(const struct order *);
void order_set_delivery_method(struct order *, struct delivery_method *dm);
int order_assign_courier(struct order *, struct courier *);
int order_change_status(struct order *, enum order_status new_status);

#endif /* ORDER_H_ */
