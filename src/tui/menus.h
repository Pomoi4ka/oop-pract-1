#ifndef MENUS_H_
#define MENUS_H_

#include "../runtime/context.h"
#include "menu.h"

struct order;

struct menu_state *yes_no_menu_create(struct context *ctx, struct menu_state *yes);
struct menu_state *client_creation_menu_create(struct context *ctx);
struct menu_state *main_menu_create(struct context *);
struct menu_state *manage_menu_create(struct context *ctx, int order_id);
struct menu_state *status_menu_create(struct context *ctx, int order_id, struct order *o);

#endif /* MENUS_H_ */
