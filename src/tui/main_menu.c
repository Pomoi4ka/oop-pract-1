#include "main_menu.h"
#include "../service/order_service.h"

#include <stdio.h>

static void new_order(struct menu *);
static void list_orders(struct menu *);
static void nothing() {}

static const struct menu_command commands[] = {
    {"new_order",   "create a new order",       new_order},
    {"list_orders", "list the existing orders", list_orders},
    {"manage",      "manage order delivery type, show price and date", nothing},
    {"assign",      "assign delivery man to the order", nothing},
    {"status",      "change order status and show current info", nothing},
    {NULL, NULL, NULL}
};

struct menu_state *main_menu_create(struct context *ctx)
{
    struct menu_state *s;
    s = context_alloc(ctx, sizeof *s);
    s->commands = commands;
    return s;
}

static void new_order(struct menu *m)
{
    struct order_service *s = menu_get_userdata(m);
    const char *client_name;

    client_name = menu_prompt(m, "client name");

    order_service_create_order(s, client_name);
}

static void list_orders(struct menu *m)
{
    struct order_service *s = menu_get_userdata(m);
    order_service_list(s);
}
