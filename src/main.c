#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "runtime/context.h"
#include "service/order_service.h"
#include "tui/menu.h"
#include "tui/main_menu.h"

#include "hashmap.h"

void run(struct context *ctx)
{
    struct menu *menu = menu_create(ctx);
    struct order_service *svc = order_service_create(ctx);
    menu_set_userdata(menu, svc);
    menu_set_state(menu, main_menu_create(ctx));
    for (;;) menu_input(menu);
}

unsigned int cstr_hasheq(enum hasheq_op op, const void *pa, const void *pb)
{
    const char *const *a, *const *b;
    a = pa;
    b = pb;
    switch (op) {
    case HASHEQ_EQ: return strcmp(*a, *b) == 0;
    case HASHEQ_HASH:
        return fnv1(*a, strlen(*a));
    }
    abort();
}

int main(void)
{
    context_enter(run);
    return 0;
}
