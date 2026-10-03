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

static const struct menu_command commands[] = {
    {"show", "show constructed client", show},
    {"name", "set client name", name},
    {"email", "set client email", email},
    {"phone", "set client phone", phone},
    {"done", "finish client creation", done},
    {NULL, NULL, NULL}
};

struct menu_state *client_creation_menu_create(struct context *ctx)
{
    struct menu_client_creation *state = context_alloc(ctx, sizeof *state);
    state->base.commands = commands;
    state->data = context_alloc(ctx, sizeof *state->data);
    return &state->base;
}

static void show(struct menu *m)
{
    struct menu_client_creation *state = (void *)menu_get_state(m);

    printf("Client:\n");
    printf("  name:  %s\n", state->data->name != NULL ? state->data->name : "(unset)");
    printf("  email: %s\n", state->data->email != NULL ? state->data->email : "(unset)");
    printf("  phone: %s\n", state->data->phone != NULL ? state->data->phone : "(unset)");
}

static void name(struct menu *m)
{
    struct menu_client_creation *state = (void*)menu_get_state(m);
    const char *value = menu_prompt(m, "name: ");
    context_free((void*)state->data->name);
    state->data->name = context_strdup(context_from_alloc(m), value);
}

static void email(struct menu *m)
{
    struct menu_client_creation *state = (void*)menu_get_state(m);
    const char *value = menu_prompt(m, "email: ");
    context_free((void*)state->data->email);
    state->data->email = context_strdup(context_from_alloc(m), value);
}

static void phone(struct menu *m)
{
    struct menu_client_creation *state = (void*)menu_get_state(m);
    const char *value = menu_prompt(m, "phone: ");
    context_free((void*)state->data->phone);
    state->data->phone = context_strdup(context_from_alloc(m), value);
}

static void done(struct menu *m)
{
    struct menu_client_creation *state = (void*)menu_get_state(m);
    struct order_service *svc = menu_get_userdata(m);
    if (!state->data->name || !*state->data->name) {
        fprintf(stderr, "* cancelled *: client name is empty\n");
        goto cleanup;
    }

    if (!client_validate_email(state->data->email)) {
        fprintf(stderr, "error: invalid email\n");
        return;
    }

    if (!client_validate_phone(state->data->phone)) {
        fprintf(stderr, "error: invalid phone\n");
        return;
    }

    if (order_service_register_client(svc, state->data)) {
        fprintf(stderr, "error: such client already in the registry\n");
    cleanup:
        context_free(state->data);
    }
    menu_pop_state(m);
    context_free(state);
}
