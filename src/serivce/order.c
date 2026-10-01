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

struct order {
    int order_id;
    enum order_status status;

    struct delivery_method *delivery_method;
    struct client_data *client;

    struct order_item *items;
    size_t items_count;
    size_t items_cap;

    time_t created_at;
    time_t estimated_at;

    struct destination_address *dest_addr;
};

struct order *order_create(struct context *ctx, int id, struct client_data *client)
{
    struct order *o = context_alloc(ctx, sizeof *o);
    o->order_id = id;
    o->client = client;
    return 0;
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

static void print_order_item(struct order_item const *item)
{
    printf("%d amount of %s, %f each\n", item->quantity, item->name, item->price);
}

void order_print(const struct order *o)
{
    size_t i;
    const char *time;

    printf("Id: %d\n", o->order_id);
    printf("Status: %s\n", order_status_as_cstr(o->status));
    printf("Delivery method: %s\n", o->delivery_method->vptr->name);
    time = ctime(&o->created_at);
    printf("Created at: %.*s\n", (int)strlen(time)-1, time);
    time = ctime(&o->estimated_at);
    printf("Estimated at: %.*s\n", (int)strlen(time)-1, time);
    /* TODO: print_destination_address(o->dest_addr); */
    printf("Ordered items:\n");
    for (i = 0; i < o->items_count; ++i) {
        print_order_item(&o->items[i]);
    }
}

void order_set_delivery_method(struct order *o, struct delivery_method *dm)
{
    assert(o->delivery_method == NULL);
    o->delivery_method = dm;
}

int order_assign_courier(struct order *o, struct courier *c)
{
    return o->delivery_method->vptr->assign_courier(o->delivery_method, c);
}

int order_change_status(struct order *o, enum order_status new_status)
{
    struct allowed_order_status_transition const *aost;
    aost = o->delivery_method->vptr->order_status_transition_table;
    for (; (int)aost->from != -1; aost++) {
        if (aost->from != o->status) continue;
        if (aost->to != new_status)  continue;
        o->status = new_status;
        return 1;
    }
    return 0;
}

size_t order_items_count(const struct order *o)
{
    return o->items_count;
}
