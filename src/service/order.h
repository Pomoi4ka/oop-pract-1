#ifndef ORDER_H_
#define ORDER_H_

#include "../runtime/context.h"
#include "courier.h"
#include "client.h"
#include "order_kind.h"

struct order;

struct order *order_create(struct context *, int id, struct client_data *);
int order_get_id(struct order const *);
void order_print(const struct order *);
void order_add_item(struct order *o, const char *name, int q, float price);
size_t order_get_items_count(const struct order *);
void order_set_order_kind(struct order *, struct order_kind *);
int order_assign_courier(struct order *, struct courier *);
int order_change_status(struct order *, enum order_status new_status);

#endif /* ORDER_H_ */
