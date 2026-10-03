#include "hashmap.h"

#include <string.h>
#include <stdlib.h>

struct hashmap {
    void *items;
    unsigned char *bitmap;
    size_t count;
    size_t cap;
    size_t itemsize;
    hasheq_f hasheq;
};

struct hashmap *hashmap_create(struct context *ctx, hasheq_f heq, size_t itemsize)
{
    struct hashmap *hm = context_alloc(ctx, sizeof *hm);
    hm->itemsize = itemsize;
    hm->hasheq = heq;
    return hm;
}

enum {
    ICTL_IS_SET,
    ICTL_SET
};

static int hashmap_index_ctl(struct hashmap *base, unsigned int index, int ctl)
{
    switch (ctl) {
    case ICTL_IS_SET:
        return !!(base->bitmap[index>>3] & (1<<(index&7)));
    case ICTL_SET:
        base->bitmap[index>>3] |= 1<<(index&7);
        return 0;
    default: abort();
    }
}

static void *hashmap_slot_at(struct hashmap *base, unsigned index)
{
    return (char *)base->items + index*base->itemsize;
}

static unsigned int hashmap_keyindex(struct hashmap *base, const void *key, int find_insert_slot)
{
    unsigned int keyhash = base->hasheq(HASHEQ_HASH, key, NULL);
    size_t limit = 4*base->cap/3;
    size_t i;

    for (i = 0; i < limit; ++i) {
        int eq = 0, isset;
        size_t index = (keyhash+i)%base->cap;
        isset = hashmap_index_ctl(base, index, ICTL_IS_SET);
        if (isset) eq = base->hasheq(HASHEQ_EQ, key, hashmap_slot_at(base, index));
        if (find_insert_slot) {
            if (eq) return index;
            if (isset) continue;
        } else {
            if (!isset) continue;
            if (!eq) continue;
        }
        return index;
    }
    return ~0;
}

void *hashmap_get(struct hashmap *base, const void *key)
{
    unsigned index = hashmap_keyindex(base, key, 0);
    if (index == ~0u) return NULL;
    return hashmap_slot_at(base, index);
}

void hashmap_resize(struct hashmap *base)
{
    void *items;
    unsigned char *bitmap;
    size_t i, cap;
    struct context *ctx;

    items = base->items;
    bitmap = base->bitmap;
    cap = base->cap;
    ctx = context_from_alloc(base);

    if (base->cap) base->cap *= 2;
    else base->cap = 1;

    base->items = context_alloc(ctx, base->cap * base->itemsize);
    base->bitmap = context_alloc(ctx, (base->cap + 7)>>3);
    base->count = 0;

    for (i = 0; i < cap; ++i) {
        if (!(bitmap[i>>3]&(1<<(i&7)))) continue;
        hashmap_insert(base, (char *)items + i*base->itemsize);
    }

    context_free(items);
    context_free(bitmap);
}

int hashmap_insert(struct hashmap *base, const void *kv)
{
    unsigned index;
    int new;
    for (;;) {
        index = hashmap_keyindex(base, kv, 1);
        if (index == ~0u) hashmap_resize(base);
        else break;
    }

    new = !hashmap_index_ctl(base, index, ICTL_IS_SET);
    hashmap_index_ctl(base, index, ICTL_SET);
    base->count += new;
    memcpy(hashmap_slot_at(base, index), kv, base->itemsize);
    return new;
}

void *hashmap_next(struct hashmap *base, size_t *iter)
{
    for (;; (*iter)++) {
        if (*iter >= base->cap) return NULL;
        if (hashmap_index_ctl(base, *iter, ICTL_IS_SET))
            return hashmap_slot_at(base, (*iter)++);
    }
}

size_t hashmap_get_count(struct hashmap *base)
{
    return base->count;
}

unsigned fnv1(const void *bytes, size_t n)
{
    unsigned fp = 16777619;
    unsigned hash = 2166136261;
    const char *b = bytes;
    const char *end = b + n;

    for (; b != end; ++b) {
        hash ^= *b;
        hash *= fp;
    }
    return hash;
}
