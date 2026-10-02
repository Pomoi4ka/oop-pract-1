#ifndef CLIENT_H_
#define CLIENT_H_

#include "../runtime/context.h"

struct client_data {
    const char *name;
};

struct client_data *client_create(struct context *, const char *name);

#endif /* CLIENT_H_ */
