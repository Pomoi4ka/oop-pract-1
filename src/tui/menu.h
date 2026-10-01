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

struct menu_state_vtable {
    /* yes, it's virtual field, it's const so it's safe :) */
    const struct menu_command *commands;
};

struct menu_state {
    struct menu_state_vtable const *vptr;
};

struct menu *menu_create(struct context *);
void menu_input(struct menu *m);
const char *menu_get_input(struct menu *m);
void menu_set_state(struct menu *m, struct menu_state *);

#endif /* MENU_H_ */
