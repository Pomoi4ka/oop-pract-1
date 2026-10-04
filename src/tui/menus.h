#ifndef MENUS_H_
#define MENUS_H_

#include "../runtime/context.h"
#include "menu.h"
#include "../service/client.h"

struct order;

struct menu_state *yes_no_menu_create(struct context *ctx,
                                      menu_action on_yes,
                                      menu_action on_no,
                                      void *userdata);
struct menu_state *client_creation_menu_create(struct context *ctx, const struct client_data *initial);
struct menu_state *main_menu_create(struct context *);
struct menu_state *new_order_menu_create(struct context *ctx);
struct menu_state *manage_menu_create(struct context *ctx, int order_id);
struct menu_state *status_menu_create(struct context *ctx, int order_id, struct order *o);
struct menu_state *items_menu_create(struct context *ctx, int order_id);

#endif /* MENUS_H_ */
