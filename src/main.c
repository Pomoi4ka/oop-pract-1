#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime/context.h"
#include "service/db.h"
#include "service/order_service.h"
#include "tui/menus.h"

void run(struct context *ctx)
{
    struct menu *menu = menu_create(ctx);
    struct order_service *svc = order_service_create(ctx);

    db_seed(ctx, svc);

    menu_set_userdata(menu, svc);
    menu_push_state(menu, main_menu_create(ctx));
    for (;;) menu_input(menu);
}

int main(void)
{
    context_enter(run);
    return 0;
}
