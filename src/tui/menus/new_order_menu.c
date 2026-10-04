#include "../menus.h"
#include "../errors.h"
#include "../../service/order_service.h"

#include <stdio.h>
#include <string.h>

struct order_draft {
    char *client_name;
    char *city;
    char *street;
    char *building;
    char *apartment;
    char *comment;
};

struct menu_new_order {
    struct menu_state base;
    struct order_draft draft;
};

static void set_client(struct menu *);
static void set_address(struct menu *);
static void show(struct menu *);
static void create(struct menu *);
static void cancel(struct menu *);

static const struct menu_command commands[] = {
    {"client",  "set client name",           set_client},
    {"address", "set delivery address",      set_address},
    {"show",    "show current draft",        show},
    {"create",  "create the order",          create},
    {"cancel",  "cancel and discard draft",  cancel},
    {NULL, NULL, NULL}
};

struct menu_state *new_order_menu_create(struct context *ctx)
{
    struct menu_new_order *s = context_alloc(ctx, sizeof *s);
    s->base.commands = commands;
    /* draft обнулён context_alloc-ом */
    return &s->base;
}

static char *dup_or_null(struct context *ctx, const char *v)
{
    if (!v || !*v) return NULL;
    return context_strdup(ctx, v);
}

static void set_client(struct menu *m)
{
    struct menu_new_order *s = (void *)menu_get_state(m);
    struct context *ctx = context_from_alloc(m);
    const char *v = menu_prompt(m, "client name: ");
    context_free(s->draft.client_name);
    s->draft.client_name = dup_or_null(ctx, v);
}

static void set_address(struct menu *m)
{
    struct menu_new_order *s = (void *)menu_get_state(m);
    struct context *ctx = context_from_alloc(m);
    const char *v;

    v = menu_prompt(m, "city: ");
    context_free(s->draft.city);
    s->draft.city = dup_or_null(ctx, v);

    v = menu_prompt(m, "street: ");
    context_free(s->draft.street);
    s->draft.street = dup_or_null(ctx, v);

    v = menu_prompt(m, "building: ");
    context_free(s->draft.building);
    s->draft.building = dup_or_null(ctx, v);

    v = menu_prompt(m, "apartment (empty to skip): ");
    context_free(s->draft.apartment);
    s->draft.apartment = dup_or_null(ctx, v);

    v = menu_prompt(m, "comment (empty to skip): ");
    context_free(s->draft.comment);
    s->draft.comment = dup_or_null(ctx, v);
}

static void show(struct menu *m)
{
    struct menu_new_order *s = (void *)menu_get_state(m);
    struct order_draft *d = &s->draft;

    printf("Draft order:\n");
    printf("  client:    %s\n", d->client_name ? d->client_name : "(unset)");
    printf("  city:      %s\n", d->city      ? d->city      : "(unset)");
    printf("  street:    %s\n", d->street    ? d->street    : "(unset)");
    printf("  building:  %s\n", d->building  ? d->building  : "(unset)");
    printf("  apartment: %s\n", d->apartment ? d->apartment : "(unset)");
    printf("  comment:   %s\n", d->comment   ? d->comment   : "(unset)");
}

static void draft_clear(struct order_draft *d)
{
    context_free(d->client_name);
    context_free(d->city);
    context_free(d->street);
    context_free(d->building);
    context_free(d->apartment);
    context_free(d->comment);
    memset(d, 0, sizeof *d);
}

static int draft_is_complete(const struct order_draft *d)
{
    return d->client_name && *d->client_name
        && d->city        && *d->city
        && d->street      && *d->street
        && d->building    && *d->building;
}

static void cancel(struct menu *m)
{
    struct menu_new_order *s = (void *)menu_get_state(m);
    draft_clear(&s->draft);
    menu_pop_state(m);
    context_free(s);
}

/* Колбэки для yes_no_menu. На момент вызова yes_no_menu уже снят,
   мы на верхушке стека — это new_order_menu. */
static void on_create_client(struct menu *m, void *userdata)
{
    struct menu_new_order *s = (void *)menu_get_state(m);
    struct context *ctx = context_from_alloc(m);
    struct client_data prefill;
    (void)userdata;

    prefill.name  = s->draft.client_name;
    prefill.email = NULL;
    prefill.phone = NULL;

    menu_push_state(m, client_creation_menu_create(ctx, &prefill));
}

static void on_skip_client(struct menu *m, void *userdata)
{
    (void)m;
    (void)userdata;
    /* просто остаёмся в new_order_menu; юзер может поправить поле
       "client" или снова нажать "create" */
}

static void create(struct menu *m)
{
    struct context *ctx = context_from_alloc(m);
    struct menu_new_order *s = (void *)menu_get_state(m);
    struct order_service *svc = menu_get_userdata(m);
    struct order *o;
    destination_address *addr;
    enum order_service_error err;

    if (!draft_is_complete(&s->draft)) {
        fprintf(stderr, "error: fields 'client', 'city', 'street', "
                        "'building' are required\n");
        return;
    }

    addr = destination_address_create(ctx,
                                      s->draft.city,
                                      s->draft.street,
                                      s->draft.building,
                                      s->draft.apartment,
                                      s->draft.comment);

    o = order_service_create_order(svc, s->draft.client_name, addr);
    if (o) {
        order_print(o);
        /* draft-строки остаются в арене и живут столько же, сколько
           адрес заказа — то есть до выхода. Не чистим, иначе addr
           в order'е будет висеть на освобождённой памяти. */
        menu_pop_state(m);
        context_free(s);
        return;
    }

    err = order_service_get_error(svc);
    if (err != OSE_CLIENT_DOESNOT_EXISTS) {
        print_order_service_error(err);
        return;
    }

    printf("Client '%s' not found. Create new client?\n", s->draft.client_name);
    menu_push_state(m, yes_no_menu_create(ctx, on_create_client, on_skip_client, NULL));
}
