#ifndef CLIENT_H_
#define CLIENT_H_

#include "../runtime/context.h"

struct client_data {
    const char *name;
};

struct client_data *client_create(struct context *, const char *name);
void client_print(struct client_data *, int pad);

#endif /* CLIENT_H_ */
