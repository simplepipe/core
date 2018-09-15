#ifndef __kernel_array_h
#define __kernel_array_h

#include <kernel/ref.h>

struct array {
        struct ref base;

        struct ref **ptr;
        unsigned len;
        unsigned reserved;
        unsigned ordered : 1;
};

struct array *array_new(const unsigned ordered);
struct array *array_init(struct array *p, const unsigned ordered);

void array_release(struct array *p);

void array_push(struct array *p, struct ref *ref);
struct ref *array_get(struct array *p, const int id);
void array_remove(struct array *p, const int id);
void array_clear(struct array *p);
void array_set(struct array *p, struct ref *ref, const int id);
void array_reserve(struct array *p, const unsigned len);
void array_truncate(struct array *p);
void array_swap(struct array *p, struct array *q);
void array_assign(struct array **p, struct array *a);
#endif
