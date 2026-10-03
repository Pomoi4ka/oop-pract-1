#ifndef YES_NO_MENU_H_
#define YES_NO_MENU_H_

#include "../runtime/context.h"

struct menu_state *yes_no_menu_create(struct context *ctx, struct menu_state *yes, struct menu_state *no);

#endif /* YES_NO_MENU_H_ */
