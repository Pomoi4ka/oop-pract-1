#ifndef ORDER_SERVICE_H_
#define ORDER_SERVICE_H_

#include "order.h"

struct order_service *order_service_create(struct context *ctx);
struct order *order_service_create_order(struct order_service *s, const char *client_name, destination_address *);
struct order *order_service_find(struct order_service *s, int id);
void order_service_list(struct order_service *s);
void order_service_set_kind(struct order_service *s, int id, order_kind *);
void order_service_assign_courier(struct order_service *s, int id, const char *name, int notes);
void order_service_change_status(struct order_service *s, int id, enum order_status st);

#endif /* ORDER_SERVICE_H_ */
