#include "../menus.h"
#include "../errors.h"
#include "../../service/order_service.h"

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

static int prompt_order_id_with_checking_the_db____wasteful_but_otherwise_ux_is_shit(struct menu *m, struct order **order)
{
    struct order *o;
    struct order_service *s;
    int id;

    s = menu_get_userdata(m);
    id = atoi(menu_prompt(m, "order id: "));
    o = order_service_find(s, id);
    if (!o) {
        print_order_service_error(order_service_get_error(s));
        if (order) *order = NULL;
        return -1;
    }
    if (order) *order = o;
    return id;
}

static void new_order(struct menu *m)
{
    struct context *ctx = context_from_alloc(m);
    menu_push_state(m, new_order_menu_create(ctx));
}

static void list_orders(struct menu *m)
{
    struct order_service *s;
    s = menu_get_userdata(m);
    order_service_list(s);
}

static void add_items(struct menu *m)
{
    struct context *ctx;
    int id;

    ctx = context_from_alloc(m);
    id = prompt_order_id_with_checking_the_db____wasteful_but_otherwise_ux_is_shit(m, NULL);
    if (id < 0) return;
    menu_push_state(m, items_menu_create(ctx, id));
}

static void manage(struct menu *m)
{
    struct context *ctx;
    struct order *o;
    int id;

    ctx = context_from_alloc(m);

    id = prompt_order_id_with_checking_the_db____wasteful_but_otherwise_ux_is_shit(m, &o);
    if (id < 0) return;
    if (!order_can_change_kind(o)) {
        fprintf(stderr, "error: order kind is frozen after packing started\n");
        return;
    }
    menu_push_state(m, manage_menu_create(ctx, id));
}

static void status(struct menu *m)
{
    struct context *ctx;
    struct order *o;
    int id;

    ctx = context_from_alloc(m);
    id = prompt_order_id_with_checking_the_db____wasteful_but_otherwise_ux_is_shit(m, &o);
    if (id < 0) return;
    order_print(o);
    menu_push_state(m, status_menu_create(ctx, id, o));
}

struct courier_stash {
    int id;
    char *name;
};

static void courier_assign_go(struct menu *m, void *userdata, int has_car)
{
    struct courier_stash *st = userdata;
    struct order_service *svc = menu_get_userdata(m);
    int notes = has_car ? COURIER_NOTES_HAS_CAR : COURIER_NOTES_NONE;

    order_service_assign_courier(svc, st->id, st->name, notes);
    if (order_service_get_error(svc) != OSE_NONE)
        print_order_service_error(order_service_get_error(svc));
    context_free(st);
}

static void courier_assign_yes(struct menu *m, void *userdata)
{
    courier_assign_go(m, userdata, 1);
}

static void courier_assign_no(struct menu *m, void *userdata)
{
    courier_assign_go(m, userdata, 0);
}

static void assign(struct menu *m)
{
    struct context *ctx;
    struct courier_stash *st;
    int id;

    ctx = context_from_alloc(m);

    id = prompt_order_id_with_checking_the_db____wasteful_but_otherwise_ux_is_shit(m, NULL);
    if (id < 0) return;

    st = context_alloc(ctx, sizeof *st);
    st->id = id;
    st->name = context_strdup(ctx, menu_prompt(m, "courier name: "));

    printf("Does the courier have a car?\n");
    menu_push_state(m, yes_no_menu_create(ctx, courier_assign_yes, courier_assign_no, st));
}
