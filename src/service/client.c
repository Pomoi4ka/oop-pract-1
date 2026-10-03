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

int client_validate_phone(const char *phone)
{
    size_t n = 0, digits = 0;
    if (!phone) return 0;
    for (; *phone; ++phone, ++n) {
        if (*phone >= '0' && *phone <= '9') { ++digits; continue; }
        if (strchr("+- ()", *phone)) continue;
        return 0;
    }
    return n > 0 && digits >= 5;
}

int client_validate_email(const char *email)
{
    const char *at, *dot;
    if (!email || !*email) return 0;
    at = strchr(email, '@');
    if (!at || at == email) return 0;
    if (strchr(at + 1, '@')) return 0;
    dot = strrchr(at + 1, '.');
    if (!dot || dot == at + 1 || !dot[1]) return 0;
    return 1;
}
