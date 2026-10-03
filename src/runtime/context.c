#include "context.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>

#define MAGIC 0xc7e3ede

struct context_alloc_hdr {
    int magic;
    struct context_alloc_hdr *next, *prev;
    struct context *parent;
};

struct context {
    struct context_alloc_hdr *allocations;
    jmp_buf exception_handler;
};

void *context_alloc(struct context *c, size_t size)
{
    struct context_alloc_hdr *hdr;
    hdr = malloc(sizeof *hdr + size);
    if (!hdr) longjmp(c->exception_handler, CTX_E_MALLOC_FAILURE);

    memset(hdr, 0, sizeof *hdr + size);
    hdr->magic = MAGIC;
    hdr->parent = c;
    if (c->allocations)
        c->allocations->prev = hdr;
    hdr->next = c->allocations;
    c->allocations = hdr;

    return hdr + 1;
}

void context_quit(struct context *c)
{
    longjmp(c->exception_handler, CTX_E_QUIT);
}

void context_enter(void (*f)(struct context *))
{
    struct context *c;
    enum context_exception e;
    c = malloc(sizeof *c);
    memset(c, 0, sizeof *c);
    if (!c) {
        fprintf(stderr, "ERROR: could not enter context\n");
        exit(1);
    }
    e = setjmp(c->exception_handler);
    switch (e) {
    case CTX_E_NONE: break;
    case CTX_E_QUIT: goto quit;
    case CTX_E_MALLOC_FAILURE:
        fprintf(stderr, "ERROR: malloc failed\n");
        exit(1);
        break;
    }

    f(c);

 quit:
    while (c->allocations) {
        void *next = c->allocations->next;
        free(c->allocations);
        c->allocations = next;
    }
    free(c);
}

static struct context_alloc_hdr *context__get_alloc_header(void *p)
{
    struct context_alloc_hdr *h = p;
    assert(h[-1].magic == MAGIC);
    return h - 1;
}

struct context *context_from_alloc(void *p)
{
    return context__get_alloc_header(p)->parent;
}

void context_free(void *p)
{
    struct context_alloc_hdr *h;
    struct context *c;

    if (!p) return;

    h = context__get_alloc_header(p);
    c = h->parent;

    if (h->next) h->next->prev = h->prev;
    if (h->prev) h->prev->next = h->next;
    if (c->allocations == h) c->allocations = h->next;

    free(h);
}

char *context_strdup(struct context *c, const char *cstr)
{
    char *copy = context_alloc(c, strlen(cstr) + 1);
    strcpy(copy, cstr);
    return copy;
}
