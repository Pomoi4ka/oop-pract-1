#include "menu.h"

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

struct menu {
    struct menu_state *state;
    void *userdata;
    char *input;
    int match;
};

static void menu__help(struct menu *m);
static void menu__quit(struct menu *m);

static const struct menu_command common_commands[] = {
    {"help", "prints this help message", menu__help},
    {"quit", "terminates program", menu__quit},
    {"exit", "", menu__quit},
    {NULL, NULL, NULL}
};

struct menu *menu_create(struct context *c)
{
    struct menu *m;
    m = context_alloc(c, sizeof *m);
    return m;
}

static void menu__on_unmatched_command(struct menu *m)
{
    if (m->match) return;
    printf("Type \"help\" for getting all commands for current menu\n");
}

static int menu__match_command(struct menu *m, struct menu_command const *cmd)
{
    for (; cmd->command; cmd++) {
        if (strcmp(m->input, cmd->command) != 0) continue;
        cmd->run(m);
        m->match = 1;
        return 1;
    }
    return 0;
}

static void menu__dispatch(struct menu *m)
{
    m->match = 0;
    if (menu__match_command(m, m->state->commands)) return;
    if (menu__match_command(m, common_commands)) return;
}

static void menu__collect_input(struct menu *m)
{
    free(m->input);
    m->input = readline("> ");
    if (!m->input) menu__quit(m);
}

const char *menu_prompt(struct menu *m, const char *prompt)
{
    printf("%s", prompt);
    menu__collect_input(m);
    return m->input;
}

void menu_input(struct menu *m)
{
    menu__on_unmatched_command(m);
    menu__collect_input(m);
    menu__dispatch(m);
}

static void menu__quit(struct menu *m)
{
    free(m->input);
    context_quit(context_from_alloc(m));
}

static void menu__help(struct menu *m)
{
    const struct menu_command *p[3], **i;
    int l, max_len = 0;
    p[0] = m->state->commands;
    p[1] = common_commands;
    p[2] = NULL;

    for (i = p; *i; ++i) {
        for (; (*i)->command; ++(*i)) {
            l = strlen((*i)->command);
            max_len = max_len < l ? l : max_len;
        }
    }

    p[0] = m->state->commands;
    p[1] = common_commands;
    p[2] = NULL;

    for (i = p; *i; ++i) {
        for (; (*i)->command; ++(*i)) {
            printf("   %*s %s\n", -max_len, (*i)->command, (*i)->description);
        }
    }
}

void menu_set_state(struct menu *m, struct menu_state *state)
{
    m->state = state;
}

void *menu_get_userdata(struct menu *m)
{
    return m->userdata;
}

void menu_set_userdata(struct menu *m, void *userdata)
{
    m->userdata = userdata;
}
