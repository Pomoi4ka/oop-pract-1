#include "main_menu.h"

#include <stdio.h>

static void new_order(struct menu *);
static void nothing() {}

static const struct menu_command commands[] = {
    {"new_order",   "create a new order",       new_order},
    {"list_orders", "list the existing orders", nothing},
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

static void new_order(struct menu *)
{
    printf("This is new order state!\n");
}
