#include "db.h"

#include <stdio.h>

#include "order.h"
#include "client.h"
#include "courier.h"
#include "destination_address.h"
#include "order_kind.h"

/* Хелпер: собрать client_data в арене и зарегистрировать в сервисе.
   Строки — литералы (.rodata), живут столько же, сколько процесс,
   поэтому их копировать в арену не нужно. */
static void seed_client(struct context *ctx,
                        struct order_service *svc,
                        const char *name,
                        const char *phone,
                        const char *email)
{
    struct client_data tmp;
    client_data *c;

    tmp.name  = name;
    tmp.phone = phone;
    tmp.email = email;

    c = client_heapify(ctx, &tmp);
    if (order_service_register_client(svc, c) != 0)
        fprintf(stderr, "[seed] duplicate client skipped: %s\n", name);
}

static destination_address *seed_address(struct context *ctx,
                                         const char *city,
                                         const char *street,
                                         const char *building)
{
    return destination_address_create(ctx, city, street, building, NULL, NULL);
}

struct seed_item {
    const char *name;
    int quantity;
    float price;
};

static struct order *seed_order(struct order_service *svc,
                                const char *client,
                                destination_address *addr,
                                order_kind *kind,
                                const struct seed_item *items,
                                size_t items_n)
{
    struct order *o;
    size_t i;
    int id;

    o = order_service_create_order(svc, client, addr);
    if (!o) {
        fprintf(stderr, "[seed] cannot create order for %s\n", client);
        return NULL;
    }

    id = order_get_id(o);
    order_service_set_kind(svc, id, kind);

    for (i = 0; i < items_n; ++i)
        order_service_add_item(svc, id, items[i].name,
                               items[i].quantity, items[i].price);

    return o;
}

void db_seed(struct context *ctx, struct order_service *svc)
{
    struct order *o;
    int id;

    /* --- Clients --------------------------------------------------- */
    seed_client(ctx, svc, "Alice", "+1-555-0101", "alice@example.com");
    seed_client(ctx, svc, "Bob",   "+1-555-0102", "bob@example.com");
    seed_client(ctx, svc, "Carol", "+1-555-0103", "carol@example.com");

    /* --- Order 1: Alice, Standard, PACKING ------------------------- */
    {
        static const struct seed_item items[] = {
            {"Coffee beans", 2, 12.50f},
            {"Milk 1L",      1,  3.20f},
        };
        destination_address *addr =
            seed_address(ctx, "Springfield", "742 Evergreen Terrace", "1");
        seed_order(svc, "Alice", addr, order_service_get_standard_kind(svc),
                   items, sizeof items / sizeof items[0]);
    }

    /* --- Order 2: Bob, Express, ON_THE_WAY (courier assigned) ------ */
    {
        static const struct seed_item items[] = {
            {"Pizza",    1, 9.90f},
            {"Cola 0.5L", 2, 1.50f},
        };
        destination_address *addr =
            seed_address(ctx, "Springfield", "123 Fake St", "5B");

        o = seed_order(svc, "Bob", addr, order_service_get_express_kind(svc),
                       items, sizeof items / sizeof items[0]);
        if (o) {
            id = order_get_id(o);
            order_service_assign_courier(svc, id, "Fast Freddie",
                                         COURIER_NOTES_HAS_CAR);
            order_service_change_status(svc, id, ORDER_STATUS_ON_THE_WAY);
        }
    }

    /* --- Order 3: Carol, Pickup, READY_FOR_PICKUP ------------------ */
    {
        static const struct seed_item items[] = {
            {"Book 'C Programming'", 1, 42.00f},
        };
        destination_address *addr =
            seed_address(ctx, "Shelbyville", "1 Main St", "10");

        o = seed_order(svc, "Carol", addr, order_service_get_pickup_kind(svc),
                       items, sizeof items / sizeof items[0]);
        if (o) {
            id = order_get_id(o);
            order_service_change_status(svc, id, ORDER_STATUS_READY_FOR_PICKUP);
        }
    }

    printf("[seed] database initialized: 3 clients, 3 orders\n");
}
