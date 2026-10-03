#include "errors.h"

#include <stdio.h>

void print_order_service_error(enum order_service_error err)
{
    fprintf(stderr, "error: service errored: ");
    switch (err) {
    case OSE_NONE:
        fprintf(stderr, "no error\n");
        break;
    case OSE_USER_DOESNOT_EXISTS:
        fprintf(stderr, "user does not exists\n");
        break;
    case OSE_NO_SUCH_ORDER_WITH_ID:
        fprintf(stderr, "no such order with the id\n");
        break;
    case OSE_COURIER_IS_NOT_SET_YET:
        fprintf(stderr, "courier is not set yet\n");
        break;
    case OSE_INVALID_NEW_STATUS:
        fprintf(stderr, "invalid new status\n");
        break;
    case OSE_KIND_DOES_NOT_NEED_COURIER:
        fprintf(stderr, "this order kind does not need a courier\n");
        break;
    case OSE_KIND_FROZEN:
        fprintf(stderr, "order kind is frozen after packing started\n");
        break;
    }
}
