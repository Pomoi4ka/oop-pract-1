#ifndef DESTINATION_ADDRESS_H_
#define DESTINATION_ADDRESS_H_

#include "../runtime/context.h"

struct destination_address {
    const char *city;
    const char *street;
    const char *building;
    const char *apartment;
    const char *comment;
};

typedef const struct destination_address destination_address;

destination_address *destination_address_create(struct context *ctx, const char *city, const char *street, const char *building, const char *apartment, const char *comment);
void destination_address_print(destination_address *, int pad);

#endif /* DESTINATION_ADDRESS_H_ */
