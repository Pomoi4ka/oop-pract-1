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
    struct client_data *client;
    struct order_item *items;
    size_t items_count;
    size_t items_cap;
    time_t created_at;
    time_t estimated_at;
    struct destination_address *dest_addr;
};

struct order {
    struct order_info info;
    struct order_kind *kind;
    enum order_status status;
    struct courier *courier;
};

struct order *order_create(struct context *ctx, int id, struct client_data *client)
{
    struct order *o = context_alloc(ctx, sizeof *o);
    o->info.id = id;
    o->info.client = client;
    o->kind = order_kind_dummy_create(ctx);
    time(&o->info.created_at);
    return o;
}

static const char *order_status_as_cstr(enum order_status status)
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
    o->kind->vptr->print(o->kind);
    time = ctime(&o->info.created_at);
    printf("Created at: %.*s\n", (int)strlen(time)-1, time);
    time = ctime(&o->info.estimated_at);
    printf("Estimated at: %.*s\n", (int)strlen(time)-1, time);
    /* TODO: print_destination_address(o->dest_addr); */
    printf("Client:\n");
    client_print(o->info.client, 4);
    printf("Ordered items:\n");
    for (i = 0; i < o->info.items_count; ++i) {
        print_order_item(&o->info.items[i], 4);
    }
}

int order_assign_courier(struct order *o, struct courier *c)
{
    if (!o->kind->vptr->needs_courier(o->kind, o)) return 0;
    o->courier = c;
    return 1;
}

int order_change_status(struct order *o, enum order_status new_status)
{
    if (!o->kind->vptr->allows_transition(o->kind, new_status))
        return 0;
    o->status = new_status;
    return 1;
}

size_t order_get_items_count(const struct order *o)
{
    return o->info.items_count;
}

int order_get_id(struct order const *o)
{
    return o->info.id;
}

const struct order_kind *order_get_order_kind(const struct order *order)
{
    return order->kind;
}

void order_set_order_kind(struct order *order, struct order_kind *kind)
{
    order->kind = kind;
}
