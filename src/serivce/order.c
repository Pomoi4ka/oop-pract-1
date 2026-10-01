#include "order.h"

#include "client.h"
#include "delivery_man.h"

struct order {
    int order_id;
    enum order_type order_type;
    enum order_status order_status;
    struct client_data *client;
    struct delivery_man *delivery_man;
    struct destination_address *dest_addr;
};
