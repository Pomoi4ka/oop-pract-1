#ifndef MENU_H_
#define MENU_H_

#include "../runtime/context.h"

struct menu;

struct menu_state;

struct menu_command {
    const char *command;
    const char *description;
    void (*run)(struct menu *);
};

struct menu_state {
    const struct menu_command *commands;
};

struct menu *menu_create(struct context *);
void menu_input(struct menu *m);
void menu_push_state(struct menu *m, struct menu_state *);
void menu_pop_state(struct menu *m);
struct menu_state *menu_get_state(struct menu *m);
void *menu_get_userdata(struct menu *m);
void menu_set_userdata(struct menu *m, void *);
const char *menu_prompt(struct menu *m, const char *prompt);

const char *menu_get_input(struct menu *m);

#endif /* MENU_H_ */
