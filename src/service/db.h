#ifndef DB_H_
#define DB_H_

#include "../runtime/context.h"
#include "order_service.h"

/* Заполняет реестры сервиса стартовым набором клиентов и заказов.
   Вызывается один раз после order_service_create(). */
void db_seed(struct context *ctx, struct order_service *svc);

#endif /* DB_H_ */
