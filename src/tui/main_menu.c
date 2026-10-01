#include "main_menu.h"

#include <stdio.h>

struct main_menu_state {
    struct menu_state_vtable const *vptr;
};

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

static const struct menu_state_vtable vtable = {
    commands
};

struct menu_state *main_menu_create(struct context *ctx)
{
    struct main_menu_state *s;
    s = context_alloc(ctx, sizeof *s);
    s->vptr = &vtable;

    return (void *) s;
}

static void new_order(struct menu *)
{
    printf("This is new order state!\n");
}
