#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "order.h"

struct order_item {
    const char *name;
    int quantity;
    float price;
};

struct order_info {
    int id;
    client_data *client;
    struct order_item *items;
    size_t items_count;
    size_t items_cap;
    time_t created_at;
    destination_address *dest_addr;
};

struct order {
    struct order_info info;
    order_kind *kind;
    enum order_status status;
    courier *courier;
};

struct order *order_create(struct context *ctx, int id, client_data *client, destination_address *addr, order_kind *kind)
{
    struct order *o = context_alloc(ctx, sizeof *o);
    o->info.id = id;
    o->info.client = client;
    o->info.dest_addr = addr;
    o->kind = kind;
    time(&o->info.created_at);
    return o;
}

const char *order_status_as_cstr(enum order_status status)
{
    switch (status) {
    case ORDER_STATUS_PACKING:          return "Packing";
    case ORDER_STATUS_READY_FOR_PICKUP: return "Ready For Pickup";
    case ORDER_STATUS_ON_THE_WAY:       return "On The Way";
    case ORDER_STATUS_DELIVERED:        return "Delivered";
    case ORDER_STATUS_PICKED:           return "Picked";
    }
    assert(0 && "unreachable");
}

void order_add_item(struct order *order, const char *name, int q, float price)
{
    struct context *c = context_from_alloc(order);
    struct order_item item;
    struct order_info *info = &order->info;

    if (info->items_count >= info->items_cap) {
        void *new_items;
        if (info->items_cap) info->items_cap *= 2;
        else info->items_cap = 1;
        new_items = context_alloc(c, sizeof *info->items * info->items_cap);
        memcpy(new_items, info->items, sizeof *info->items * info->items_count);
        context_free(info->items);
        info->items = new_items;
    }

    item.name = name;
    item.quantity = q;
    item.price = price;

    info->items[info->items_count++] = item;
}

static void print_order_item(struct order_item const *item, int pad)
{
    printf("%*s%d amount of %s, %f each\n", pad, "", item->quantity, item->name, item->price);
}

void order_print(const struct order *o)
{
    size_t i;
    const char *time;

    printf("Id: %d\n", o->info.id);
    printf("Status: %s\n", order_status_as_cstr(o->status));
    printf("Delivery type: %s\n", o->kind->name);
    time = ctime(&o->info.created_at);
    printf("Created at: %.*s\n", (int)strlen(time)-1, time);
    printf("ETA: %.0f h\n", order_calc_eta_hours(o));
    printf("Cost: %.2f\n", order_calc_cost(o));
    printf("Client:\n");
    client_print(o->info.client, 4);
    printf("Ordered items:\n");
    for (i = 0; i < o->info.items_count; ++i) {
        print_order_item(&o->info.items[i], 4);
    }
    if (o->courier) courier_print(o->courier, 0);
    destination_address_print(o->info.dest_addr, 0);
}

int order_assign_courier(struct order *o, courier *c)
{
    if (!o->kind->vptr->needs_courier(o->kind, o)) return 0;
    o->courier = c;
    return 1;
}

const char *order_status_transition_result_as_cstr(enum order_status_transition_result r)
{
    switch (r) {
    case OSTR_SUCCESS:
        return "success";
    case OSTR_COURIER_IS_NOT_SET_YET:
        return "courier is not set yet";
    case OSTR_INVALID_NEW_STATUS_FOR_THIS_KIND_OF_ORDER:
        return "invalid new status for this kind of order";
    }
    assert(0 && "unreachable");
}

enum order_status_transition_result order_change_status(struct order *o, enum order_status new_status)
{
    enum order_status_transition_result ret;
    ret = o->kind->vptr->allows_transition(o->kind, o, new_status);
    if (ret != OSTR_SUCCESS) return ret;
    o->status = new_status;
    return OSTR_SUCCESS;
}

size_t order_get_items_count(const struct order *o)
{
    return o->info.items_count;
}

int order_get_id(struct order const *o)
{
    return o->info.id;
}

void order_set_order_kind(struct order *order, order_kind *kind)
{
    order->kind = kind;
}

enum order_status order_get_status(const struct order *order)
{
    return order->status;
}

courier *order_get_courier(const struct order *order)
{
    return order->courier;
}

double order_calc_cost(const struct order *o)
{
    return (double)o->kind->vptr->calc_cost(o->kind, (struct order *)o);
}

double order_calc_eta_hours(const struct order *o)
{
    return (double)o->kind->vptr->calc_estimated_time(o->kind, (struct order *)o);
}

int order_can_change_status(const struct order *o, enum order_status to)
{
    return o->kind->vptr->allows_transition(o->kind, (struct order *)o, to) == OSTR_SUCCESS;
}
