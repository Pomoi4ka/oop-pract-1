#ifndef HASHMAP_H_
#define HASHMAP_H_

#include "runtime/context.h"

enum hasheq_op {
    HASHEQ_HASH,
    HASHEQ_EQ
};

struct hashmap;

typedef unsigned int (*hasheq_f)(enum hasheq_op, const void *, const void *);

struct hashmap *hashmap_create(struct context *ctx, hasheq_f, size_t itemsize);
void *hashmap_get(struct hashmap *, const void *);
int   hashmap_insert(struct hashmap *, const void *); /* = 1 if element is new - count is modified */
void *hashmap_next(struct hashmap *, size_t *iter);
size_t hashmap_get_count(struct hashmap *);
unsigned fnv1(const void *, size_t);

#endif /* HASHMAP_H_ */
