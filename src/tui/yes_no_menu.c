#include "menu.h"

struct menu_yes_no {
    struct menu_state base;
    menu_action on_yes;
    menu_action on_no;
    void *userdata;
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

struct menu_state *yes_no_menu_create(struct context *ctx,
                                      menu_action on_yes,
                                      menu_action on_no,
                                      void *userdata)
{
    struct menu_yes_no *state = context_alloc(ctx, sizeof *state);
    state->base.commands = commands;
    state->on_yes = on_yes;
    state->on_no = on_no;
    state->userdata = userdata;
    return &state->base;
}

static void yes(struct menu *m)
{
    struct menu_yes_no *state = (void *)menu_get_state(m);
    menu_pop_state(m);
    if (state->on_yes) state->on_yes(m, state->userdata);
    context_free(state);
}

static void no(struct menu *m)
{
    struct menu_yes_no *state = (void *)menu_get_state(m);
    menu_pop_state(m);
    if (state->on_no) state->on_no(m, state->userdata);
    context_free(state);
}
