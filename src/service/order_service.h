#ifndef ORDER_SERVICE_H_
#define ORDER_SERVICE_H_

#include "order.h"

enum order_service_error {
    OSE_NONE,
    OSE_USER_DOESNOT_EXISTS,
    OSE_NO_SUCH_ORDER_WITH_ID,
    OSE_INVALID_NEW_STATUS
};

struct order_service *order_service_create(struct context *ctx);
struct order *order_service_create_order(struct order_service *s, const char *client_name, destination_address *);
struct order *order_service_find(struct order_service *s, int id);
int  order_service_register_client(struct order_service *s, client_data *);
void order_service_list(struct order_service *s);
void order_service_set_kind(struct order_service *s, int id, order_kind *);
void order_service_assign_courier(struct order_service *s, int id, const char *name, int notes);
void order_service_change_status(struct order_service *s, int id, enum order_status st);
enum order_service_error order_service_get_error(struct order_service *s);

#endif /* ORDER_SERVICE_H_ */
