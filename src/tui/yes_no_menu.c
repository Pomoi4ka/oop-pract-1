#include "menu.h"

struct menu_yes_no {
    struct menu_state base;
    struct menu_state *yes;
};

static void yes(struct menu *);
static void no(struct menu *);

static const struct menu_command commands[] = {
    {"y",         "yes answer", yes},
    {"yes",       "", yes},
    {"да",        "", yes},
    {"ok",        "", yes},
    {"of course", "", yes},
    {"YEAH",      "", yes},
    {"n",         "no answer", no},
    {"нет",       "", no},
    {"no",        "", no},
    {"NO",        "", no},
    {"not ok",    "", no},
    {NULL, NULL, NULL}
};

struct menu_state *yes_no_menu_create(struct context *ctx, struct menu_state *yes)
{
    struct menu_yes_no *state = context_alloc(ctx, sizeof *state);
    state->base.commands = commands;
    state->yes = yes;
    return &state->base;
}

static void yes(struct menu *m)
{
    struct menu_yes_no *state = (void *)menu_get_state(m);
    menu_pop_state(m);
    menu_push_state(m, state->yes);
    context_free(state);
}

static void no(struct menu *m)
{
    struct menu_yes_no *state = (void *)menu_get_state(m);
    menu_pop_state(m);
    context_free(state->yes);
    context_free(state);
}
