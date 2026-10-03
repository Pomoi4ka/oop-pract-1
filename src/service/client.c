#include "client.h"

#include <stdio.h>
#include <string.h>

client_data *client_heapify(struct context *ctx, client_data *cdata)
{
    struct client_data *data = context_alloc(ctx, sizeof *data);
    memcpy(data, cdata, sizeof *data);
    return data;
}

void client_print(client_data *client, int pad)
{
    printf("%*sName: %s\n", pad, "", client->name);
    printf("%*sPhone: %s\n", pad, "", client->phone);
    printf("%*sEmail: %s\n", pad, "", client->email);
}

unsigned client_hasheq(enum hasheq_op op, const void *pa, const void *pb)
{
    client_data *const *a, *const *b;
    a = pa;
    b = pb;
    switch (op) {
    case HASHEQ_EQ:
        return strcmp((*a)->name, (*b)->name) == 0;
    case HASHEQ_HASH:
        return fnv1((*a)->name, strlen((*a)->name));
    }
    return -1;
}
