#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <stddef.h>

struct context;

enum context_exception {
    CTX_E_NONE,
    CTX_E_QUIT,
    CTX_E_MALLOC_FAILURE
};

void context_enter(void (*)(struct context *));
void context_quit(struct context *c);
int context_quitting(struct context *c);
void *context_alloc(struct context *c, size_t size);
char *context_strdup(struct context *c, const char *);
void context_free(void *);
struct context *context_from_alloc(void *);

#endif /* CONTEXT_H_ */
