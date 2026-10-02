#include "client.h"

struct client_data *client_create(struct context *ctx, const char *name)
{
    struct client_data *data;
    data = context_alloc(ctx, sizeof *data);
    data->name = name;
    return data;
}
