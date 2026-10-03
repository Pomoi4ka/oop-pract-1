#include "menus.h"
#include "errors.h"
#include "../service/order_service.h"

#include <stdio.h>
#include <stdlib.h>

static void new_order(struct menu *);
static void list_orders(struct menu *);
static void add_items(struct menu *);
static void manage(struct menu *);
static void assign(struct menu *);
static void status(struct menu *);

static const struct menu_command commands[] = {
    {"new_order",   "create a new order",                          new_order},
    {"list_orders", "list the existing orders",                    list_orders},
    {"items",       "add items to an order",                       add_items},
    {"manage",      "choose delivery kind, show cost and ETA",     manage},
    {"assign",      "assign courier to the order",                 assign},
    {"status",      "change order status and show current info",   status},
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
    struct context *ctx;
    struct order_service *s;
    struct order *order;
    const char *client_name;
    destination_address *dest;
    enum order_service_error err;
    struct menu_state *question;

    ctx = context_from_alloc(m);
    s = (struct order_service *)menu_get_userdata(m);

    client_name = menu_prompt(m, "client name: ");
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
    struct order_service *s;
    s = (struct order_service *)menu_get_userdata(m);
    order_service_list(s);
}

static void add_items(struct menu *m)
{
    struct context *ctx;
    struct order_service *s;
    const char *id_str;
    struct order *o;
    int id;

    ctx = context_from_alloc(m);
    s = (struct order_service *)menu_get_userdata(m);

    id_str = menu_prompt(m, "order id: ");
    id = atoi(id_str);
    o = order_service_find(s, id);
    if (!o) {
        print_order_service_error(order_service_get_error(s));
        return;
    }
    menu_push_state(m, items_menu_create(ctx, id));
}

static void manage(struct menu *m)
{
    struct context *ctx;
    struct order_service *s;
    const char *id_str;
    struct order *o;
    int id;

    ctx = context_from_alloc(m);
    s = (struct order_service *)menu_get_userdata(m);

    id_str = menu_prompt(m, "order id: ");
    id = atoi(id_str);
    o = order_service_find(s, id);
    if (!o) {
        print_order_service_error(order_service_get_error(s));
        return;
    }
    menu_push_state(m, manage_menu_create(ctx, id));
}

static void status(struct menu *m)
{
    struct context *ctx;
    struct order_service *s;
    const char *id_str;
    struct order *o;
    int id;

    ctx = context_from_alloc(m);
    s = (struct order_service *)menu_get_userdata(m);

    id_str = menu_prompt(m, "order id: ");
    id = atoi(id_str);
    o = order_service_find(s, id);
    if (!o) {
        print_order_service_error(order_service_get_error(s));
        return;
    }
    order_print(o);
    menu_push_state(m, status_menu_create(ctx, id, o));
}

static void assign(struct menu *m)
{
    struct order_service *s;
    const char *id_str;
    char *name;
    const char *car;
    int id, notes;

    s = (struct order_service *)menu_get_userdata(m);

    id_str = menu_prompt(m, "order id: ");
    id = atoi(id_str);

    /* menu_prompt reuses m->input across calls; copy the name */
    name = context_strdup(context_from_alloc(m), menu_prompt(m, "courier name: "));
    car  = menu_prompt(m, "has car? (y/n): ");

    notes = COURIER_NOTES_NONE;
    if (car[0] == 'y' || car[0] == 'Y') notes |= COURIER_NOTES_HAS_CAR;

    order_service_assign_courier(s, id, name, notes);
    if (order_service_get_error(s) != OSE_NONE)
        print_order_service_error(order_service_get_error(s));
}
