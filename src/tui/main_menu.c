#include "menus.h"
#include "../service/order_service.h"

#include <stdio.h>

static void new_order(struct menu *);
static void list_orders(struct menu *);
static void nothing() {}

static const struct menu_command commands[] = {
    {"new_order",   "create a new order",       new_order},
    {"list_orders", "list the existing orders", list_orders},
    {"manage",      "manage order delivery type, show price and date", nothing},
    {"assign",      "assign courier to the order", nothing},
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

static void print_order_service_error(enum order_service_error err)
{
    fprintf(stderr, "error: service errored: ");
    switch (err) {
    case OSE_NONE: fprintf(stderr, "no error\n"); break;
    case OSE_USER_DOESNOT_EXISTS: fprintf(stderr, "user does not exists\n"); break;
    case OSE_NO_SUCH_ORDER_WITH_ID: fprintf(stderr, "no such order with the id\n"); break;
    case OSE_INVALID_NEW_STATUS: fprintf(stderr, "invalid new status\n"); break;
    }
}

static void new_order(struct menu *m)
{
    struct context *ctx = context_from_alloc(m);
    struct order_service *s = menu_get_userdata(m);
    struct order *order;
    const char *client_name;
    destination_address *dest;
    enum order_service_error err;
    struct menu_state *question;

    /* TODO: more complex prompts for creating a user and the address */
    client_name = menu_prompt(m, "client name");
    dest        = destination_address_create(ctx, "TODO-City", "TODO-Street", "TODO-building", NULL, NULL);

    order = order_service_create_order(s, client_name, dest);
    if (order) {
        order_print(order);
        return;
    }
    err = order_service_get_error(s);
    print_order_service_error(err);
    if (err != OSE_USER_DOESNOT_EXISTS) return;

    printf("Create new user?\n");
    question = yes_no_menu_create(ctx, client_creation_menu_create(ctx));
    menu_push_state(m, question);
}

static void list_orders(struct menu *m)
{
    struct order_service *s = menu_get_userdata(m);
    order_service_list(s);
}
