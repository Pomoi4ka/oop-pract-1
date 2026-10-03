#include "../menus.h"
#include "../errors.h"
#include "../../service/order_service.h"

#include <stdio.h>

struct menu_manage {
    struct menu_state base;
    int order_id;
};

static void pick_standard(struct menu *);
static void pick_express(struct menu *);
static void pick_pickup(struct menu *);

static const struct menu_command commands[] = {
    {"standard", "standard delivery", pick_standard},
    {"express",  "express delivery",  pick_express},
    {"pickup",   "self pickup",       pick_pickup},
    {NULL, NULL, NULL}
};

static void finish(struct menu *m, order_kind *k)
{
    struct menu_manage *s;
    struct order_service *svc;
    struct order *o;

    s = (struct menu_manage *)menu_get_state(m);
    svc = (struct order_service *)menu_get_userdata(m);

    order_service_set_kind(svc, s->order_id, k);
    if (order_service_get_error(svc) != OSE_NONE) {
        print_order_service_error(order_service_get_error(svc));
        menu_pop_state(m);
        context_free(s);
        return;
    }

    o = order_service_find(svc, s->order_id);
    if (o) {
        printf("Delivery: %s\n", k->name);
        printf("Cost:     %.2f\n", order_calc_cost(o));
        printf("ETA:      %.0f h\n", order_calc_eta_hours(o));
    }

    menu_pop_state(m);
    context_free(s);
}

static void pick_standard(struct menu *m)
{
    struct order_service *svc;
    svc = (struct order_service *)menu_get_userdata(m);
    finish(m, order_service_get_standard_kind(svc));
}

static void pick_express(struct menu *m)
{
    struct order_service *svc;
    svc = (struct order_service *)menu_get_userdata(m);
    finish(m, order_service_get_express_kind(svc));
}

static void pick_pickup(struct menu *m)
{
    struct order_service *svc;
    svc = (struct order_service *)menu_get_userdata(m);
    finish(m, order_service_get_pickup_kind(svc));
}

struct menu_state *manage_menu_create(struct context *ctx, int order_id)
{
    struct menu_manage *s;
    s = context_alloc(ctx, sizeof *s);
    s->base.commands = commands;
    s->order_id = order_id;
    return &s->base;
}
