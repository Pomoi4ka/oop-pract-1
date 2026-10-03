#include "client.h"

#include <stdio.h>

client_data *client_create(struct context *ctx, const char *name, const char *phone, const char *email)
{
    struct client_data *data;
    data = context_alloc(ctx, sizeof *data);
    data->name = name;
    data->phone = phone;
    data->email = email;
    return data;
}

void client_print(client_data *client, int pad)
{
    printf("%*sName: %s\n", pad, "", client->name);
    printf("%*sPhone: %s\n", pad, "", client->phone);
    printf("%*sEmail: %s\n", pad, "", client->email);
}
