#include "menus.h"
#include "errors.h"
#include "../service/order_service.h"

#include <stdio.h>
#include <string.h>

static const struct status_entry {
    const char *cmd;
    const char *desc;
    enum order_status status;
} status_table[] = {
    {"packing",          "back to packing",  ORDER_STATUS_PACKING},
    {"ready_for_pickup", "ready for pickup", ORDER_STATUS_READY_FOR_PICKUP},
    {"on_the_way",       "start delivery",   ORDER_STATUS_ON_THE_WAY},
    {"delivered",        "mark delivered",   ORDER_STATUS_DELIVERED},
    {"picked",           "mark picked up",   ORDER_STATUS_PICKED}
};

#define STATUS_TABLE_N (sizeof status_table / sizeof status_table[0])

struct menu_status {
    struct menu_state base;
    int order_id;
    struct menu_command cmds[STATUS_TABLE_N + 2];
};

static void change_status(struct menu *m)
{
    struct menu_status *s;
    struct order_service *svc;
    struct order *o;
    const char *input;
    size_t i;

    s = (struct menu_status *)menu_get_state(m);
    svc = (struct order_service *)menu_get_userdata(m);
    input = menu_get_input(m);

    for (i = 0; i < STATUS_TABLE_N; ++i) {
        if (strcmp(input, status_table[i].cmd) == 0) break;
    }
    if (i == STATUS_TABLE_N) return;

    order_service_change_status(svc, s->order_id, status_table[i].status);
    if (order_service_get_error(svc) != OSE_NONE)
        print_order_service_error(order_service_get_error(svc));

    o = order_service_find(svc, s->order_id);
    if (o) order_print(o);

    menu_pop_state(m);
    context_free(s);
}

static void cancel(struct menu *m)
{
    struct menu_status *s = (struct menu_status *)menu_get_state(m);
    menu_pop_state(m);
    context_free(s);
}

struct menu_state *status_menu_create(struct context *ctx, int order_id, struct order *o)
{
    struct menu_status *s;
    size_t i, n;

    s = context_alloc(ctx, sizeof *s);

    n = 0;
    for (i = 0; i < STATUS_TABLE_N; ++i) {
        if (!order_can_change_status(o, status_table[i].status)) continue;
        s->cmds[n].command     = status_table[i].cmd;
        s->cmds[n].description = status_table[i].desc;
        s->cmds[n].run         = change_status;
        ++n;
    }
    s->cmds[n].command     = "cancel";
    s->cmds[n].description = "cancel and return";
    s->cmds[n].run         = cancel;
    ++n;
    s->cmds[n].command     = NULL;
    s->cmds[n].description = NULL;
    s->cmds[n].run         = NULL;

    s->base.commands = s->cmds;
    s->order_id = order_id;
    return &s->base;
}
