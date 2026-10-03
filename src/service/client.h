#ifndef CLIENT_H_
#define CLIENT_H_

#include "../runtime/context.h"

struct client_data {
    const char *name;
    const char *phone;
    const char *email;
};

typedef const struct client_data client_data;

client_data *client_create(struct context *, const char *name, const char *phone, const char *email);
int client_validate_phone(const char *phone);
int client_validate_email(const char *email);
void client_print(client_data *, int pad);

#endif /* CLIENT_H_ */
