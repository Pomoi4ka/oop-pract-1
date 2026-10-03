#include "destination_address.h"

#include <stdio.h>

destination_address *destination_address_create(struct context *ctx, const char *city, const char *street, const char *building, const char *apartment, const char *comment)
{
    struct destination_address *dest = context_alloc(ctx, sizeof *dest);
    dest->city = city;
    dest->street = street;
    dest->building = building;
    dest->apartment = apartment;
    dest->comment = comment;
    return dest;
}

void destination_address_print(destination_address *addr, int pad)
{
    printf("%*sCity: %s\n", pad, "", addr->city);
    printf("%*sStreet: %s\n", pad, "", addr->street);
    printf("%*sBuilding: %s\n", pad, "", addr->building);
    if (addr->apartment) printf("%*sApartment: %s\n", pad, "", addr->apartment);
    if (addr->comment) printf("%*sComment:\n%*s%s", pad, "", pad+2, "", addr->comment);
}
