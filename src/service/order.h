#ifndef ORDER_H_
#define ORDER_H_

#include "../runtime/context.h"
#include "courier.h"
#include "client.h"
#include "destination_address.h"

struct order;

enum order_status_transition_result {
    OSTR_SUCCESS,
    OSTR_COURIER_IS_NOT_SET_YET,
    OSTR_INVALID_NEW_STATUS_FOR_THIS_KIND_OF_ORDER
};

#include "order_kind.h"

const char *order_status_transition_result_as_cstr(enum order_status_transition_result);

struct order *order_create(struct context *, int id, client_data *, destination_address *, order_kind *);
void order_print(const struct order *);
void order_add_item(struct order *o, const char *name, int q, float price);
void order_set_order_kind(struct order *, order_kind *);
int order_assign_courier(struct order *, courier *);
enum order_status_transition_result order_change_status(struct order *, enum order_status new_status);

int order_get_id(struct order const *);
size_t order_get_items_count(const struct order *);
enum order_status order_get_status(const struct order *);
courier *order_get_courier(const struct order *);

double order_calc_cost(const struct order *);
double order_calc_eta_hours(const struct order *);
int order_can_change_status(const struct order *, enum order_status);
const char *order_status_as_cstr(enum order_status);

#endif /* ORDER_H_ */
