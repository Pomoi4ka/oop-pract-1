#include "../menus.h"
#include "../../service/order_service.h"

#include <stdio.h>
#include <stdlib.h>

struct menu_items {
    struct menu_state base;
    int order_id;
};

static void add(struct menu *);
static void done(struct menu *);

static const struct menu_command commands[] = {
    {"add",  "add a new item to the order", add},
    {"done", "finish adding items",         done},
    {NULL, NULL, NULL}
};

struct menu_state *items_menu_create(struct context *ctx, int order_id)
{
    struct menu_items *s = context_alloc(ctx, sizeof *s);
    s->base.commands = commands;
    s->order_id = order_id;
    return &s->base;
}

static void add(struct menu *m)
{
    struct menu_items *s = (void *)menu_get_state(m);
    struct order_service *svc = menu_get_userdata(m);
    const char *qty_s, *price_s;
    char *name;
    int qty;
    float price;

    /* menu_prompt reuses m->input across calls; copy the name */
    name = context_strdup(context_from_alloc(m), menu_prompt(m, "item name: "));

    qty_s = menu_prompt(m, "quantity: ");
    qty = atoi(qty_s);
    if (qty <= 0) {
        fprintf(stderr, "error: quantity must be positive\n");
        return;
    }

    price_s = menu_prompt(m, "price: ");
    price = (float)atof(price_s);
    if (price < 0.0f) {
        fprintf(stderr, "error: price must be non-negative\n");
        return;
    }

    order_service_add_item(svc, s->order_id, name, qty, price);
    printf("Item added.\n");
}

static void done(struct menu *m)
{
    struct menu_items *s = (void *)menu_get_state(m);
    struct order_service *svc = menu_get_userdata(m);
    struct order *o = order_service_find(svc, s->order_id);

    if (o) order_print(o);

    menu_pop_state(m);
    context_free(s);
}
