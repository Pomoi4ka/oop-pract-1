#include "../menus.h"
#include "../../service/client.h"
#include "../../service/order_service.h"

#include <stdio.h>

struct menu_client_creation {
    struct menu_state base;
    struct client_data *data;
};

static void show(struct menu *);
static void name(struct menu *);
static void email(struct menu *);
static void phone(struct menu *);
static void done(struct menu *);
static void cancel(struct menu *);

static const struct menu_command commands[] = {
    {"show",   "show constructed client", show},
    {"name",   "set client name",         name},
    {"email",  "set client email",        email},
    {"phone",  "set client phone",        phone},
    {"done",   "finish client creation",  done},
    {"cancel", "abort and return",        cancel},
    {NULL, NULL, NULL}
};

static char *opt_dup(struct context *ctx, const char *v)
{
    if (!v || !*v) return NULL;
    return context_strdup(ctx, v);
}

struct menu_state *client_creation_menu_create(struct context *ctx, const struct client_data *initial)
{
    struct menu_client_creation *state = context_alloc(ctx, sizeof *state);
    state->base.commands = commands;
    state->data = context_alloc(ctx, sizeof *state->data);

    if (initial) {
        state->data->name  = opt_dup(ctx, initial->name);
        state->data->email = opt_dup(ctx, initial->email);
        state->data->phone = opt_dup(ctx, initial->phone);
    }

    return &state->base;
}

static void show(struct menu *m)
{
    struct menu_client_creation *state = (void *)menu_get_state(m);

    printf("Client:\n");
    printf("  name:  %s\n", state->data->name  ? state->data->name  : "(unset)");
    printf("  email: %s\n", state->data->email ? state->data->email : "(unset)");
    printf("  phone: %s\n", state->data->phone ? state->data->phone : "(unset)");
}

static void name(struct menu *m)
{
    struct menu_client_creation *state = (void *)menu_get_state(m);
    const char *v = menu_prompt(m, "name: ");
    context_free((void *)state->data->name);
    state->data->name = opt_dup(context_from_alloc(m), v);
}

static void email(struct menu *m)
{
    struct menu_client_creation *state = (void *)menu_get_state(m);
    const char *v = menu_prompt(m, "email: ");
    context_free((void *)state->data->email);
    state->data->email = opt_dup(context_from_alloc(m), v);
}

static void phone(struct menu *m)
{
    struct menu_client_creation *state = (void *)menu_get_state(m);
    const char *v = menu_prompt(m, "phone: ");
    context_free((void *)state->data->phone);
    state->data->phone = opt_dup(context_from_alloc(m), v);
}

static void done(struct menu *m)
{
    struct menu_client_creation *state = (void *)menu_get_state(m);
    struct order_service *svc = menu_get_userdata(m);
    struct client_data *cd = state->data;

    if (!cd->name || !*cd->name) {
        fprintf(stderr, "error: name is required\n");
        return;
    }
    if (!client_validate_email(cd->email)) {
        fprintf(stderr, "error: invalid email (expected name@domain.tld)\n");
        return;
    }
    if (!client_validate_phone(cd->phone)) {
        fprintf(stderr, "error: invalid phone (at least 5 digits, allowed: digits, + - space ( ))\n");
        return;
    }
    if (order_service_register_client(svc, cd)) {
        fprintf(stderr, "error: client '%s' already exists; pick a different name\n", cd->name);
        return;
    }

    printf("Client '%s' registered. Type 'create' to finish the order, or 'quit' to exit.\n", cd->name);

    menu_pop_state(m);
    context_free(state);
}

static void cancel(struct menu *m)
{
    struct menu_client_creation *state = (void *)menu_get_state(m);
    /* state->data и его строки остаются в арене до выхода из контекста;
       освобождать их не критично, но чтобы не плодить мусор — чистим. */
    context_free((void *)state->data->name);
    context_free((void *)state->data->email);
    context_free((void *)state->data->phone);
    context_free(state->data);
    menu_pop_state(m);
    context_free(state);
}
