#ifndef MENUS_H_
#define MENUS_H_

#include "../runtime/context.h"
#include "menu.h"

struct menu_state *yes_no_menu_create(struct context *ctx, struct menu_state *yes);
struct menu_state *client_creation_menu_create(struct context *ctx);
struct menu_state *main_menu_create(struct context *);

#endif /* MENUS_H_ */
