#include "order_service.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "../hashmap.h"

struct client_registry {
    struct hashmap *map;
};

struct courier_registry {
    struct hashmap *map;
};

struct order_service {
    struct context *ctx;

    struct client_registry client_registry;
    struct courier_registry courier_registry;

    /* Singletons */
    order_kind *standard;
    order_kind *express;

    struct order **orders;
    size_t count, cap;
    int next_id;

    enum order_service_error err;
};

enum order_service_error order_service_get_error(struct order_service *s)
{
    return s->err;
}

struct order *order_service_find(struct order_service *s, int id)
{
    size_t mid, lo, hi;

    s->err = OSE_NONE;
    lo = 0;
    hi = s->count;

    while (lo < hi) {
        int c_id;
        mid = lo + (hi - lo)/2;
        c_id = order_get_id(s->orders[mid]);
        if (c_id > id) lo = mid + 1;
        else if (c_id < id) hi = mid;
        else return s->orders[mid];
    }
    s->err = OSE_NO_SUCH_ORDER_WITH_ID;
    return NULL;
}

struct order_service *order_service_create(struct context *ctx)
{
    struct order_service *svc;
    svc = context_alloc(ctx, sizeof *svc);
    svc->ctx = ctx;
    /* this is stupid, but the data should not move, so its kinda makes sense */
    svc->client_registry.map = hashmap_create(ctx, client_hasheq, sizeof(client_data*));
    svc->courier_registry.map = hashmap_create(ctx, courier_hasheq, sizeof(courier*));
    svc->standard = order_kind_standard_create(ctx);
    svc->express = order_kind_express_create(ctx);
    svc->next_id = 1;
    return svc;
}

void order_service_set_kind(struct order_service *s, int id, order_kind *kind)
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
    struct order *order = order_service_find(s, id);
    if (!order) return;

    switch (order_change_status(order, st)) {
    case OSTR_SUCCESS: break;
    case OSTR_COURIER_IS_NOT_SET_YET:
        s->err = OSE_COURIER_IS_NOT_SET_YET;
        break;
    case OSTR_INVALID_NEW_STATUS_FOR_THIS_KIND_OF_ORDER:
        s->err = OSE_INVALID_NEW_STATUS;
        break;
    }
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

client_data *order_service_find_client(struct client_registry *reg, const char *client_name)
{
    struct client_data key = {0};
    client_data *const *c;
    struct client_data *pkey;
    key.name = client_name;
    pkey = &key;
    c = hashmap_get(reg->map, &pkey);
    if (!c) return NULL;
    return *c;
}

struct order *order_service_create_order(struct order_service *s, const char *client_name, destination_address *addr)
{
    struct order *order;
    client_data *client;

    client = order_service_find_client(&s->client_registry, client_name);
    if (!client) {
        s->err = OSE_USER_DOESNOT_EXISTS;
        return NULL;
    }

    order = order_create(s->ctx, s->next_id++, client, addr, s->standard);
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

int order_service_register_client(struct order_service *s, client_data *cd)
{
    client_data *d = order_service_find_client(&s->client_registry, cd->name);
    if (d) return 1;
    hashmap_insert(s->client_registry.map, &cd);
    return 0;
}
