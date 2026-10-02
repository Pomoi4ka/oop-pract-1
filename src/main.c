#include <stdio.h>

#include "runtime/context.h"
#include "service/order_service.h"
#include "tui/menu.h"
#include "tui/main_menu.h"

void run(struct context *ctx)
{
    struct menu *menu = menu_create(ctx);
    struct order_service *svc = order_service_create(ctx);
    menu_set_userdata(menu, svc);
    menu_set_state(menu, main_menu_create(ctx));
    for (;;) menu_input(menu);
}

int main(void)
{
    context_enter(run);
    return 0;
}
