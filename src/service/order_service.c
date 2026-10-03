#include "order_service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int order_compare(const void *pa, const void *pb)
{
    const struct order *a, *b;
    a = pa;
    b = pb;

    return order_get_id(b) - order_get_id(a);
}

struct order *order_service_find(struct order_service *s, int id)
{
    struct order *order;
    order = bsearch(&id, s->orders, s->count, s->count * sizeof *s->orders, order_compare);
    if (!order) fprintf(stderr, "error: failed to find order with id %d\n", id);
    return order;
}

struct order_service *order_service_create(struct context *ctx)
{
    struct order_service *svc;
    svc = context_alloc(ctx, sizeof *svc);
    svc->ctx = ctx;
    svc->standard = order_kind_standard_create(ctx);
    svc->express = order_kind_express_create(ctx);
    svc->next_id = 1;
    return svc;
}

void order_service_set_delivery(struct order_service *s, int id, struct order_kind *kind)
{
    struct order *order = order_service_find(s, id);
    if (!order) return;

    order_set_order_kind(order, kind);
}

void order_service_assign_courier(struct order_service *s, int id, const char *name, int notes)
{
    struct order *order = order_service_find(s, id);
    courier *c;
    if (!order) return;

    /* TODO: couriers registry */

    c = courier_create(s->ctx, name, notes);
    order_assign_courier(order, c);
}

void order_service_change_status(struct order_service *s, int id, enum order_status st)
{
    int ret;
    struct order *order = order_service_find(s, id);
    if (!order) return;

    ret = order_change_status(order, st);
    if (ret) return;
    fprintf(stderr, "error: failed to change status\n");
}

void order_service_add_order(struct order_service *s, struct order *order)
{
    if (s->count >= s->cap) {
        void *new_orders;
        if (s->cap) s->cap *= 2;
        else s->cap = 1;
        new_orders = context_alloc(s->ctx, sizeof *s->orders * s->cap);
        memcpy(new_orders, s->orders, sizeof *s->orders * s->count);
        context_free(s->orders);
        s->orders = new_orders;
    }

    s->orders[s->count++] = order;
}

struct order *order_service_create_order(struct order_service *s, const char *client_name)
{
    struct order *order;
    struct client_data *client;

    /* TODO: clients registry */

    client_name = context_strcpy(s->ctx, client_name);
    client = client_create(s->ctx, client_name);
    order = order_create(s->ctx, s->next_id++, client, s->standard);

    order_service_add_order(s, order);

    return order;
}

void order_service_list(struct order_service *s)
{
    size_t i;
    for (i = 0; i < s->count; ++i) {
        order_print(s->orders[i]);
    }
}
