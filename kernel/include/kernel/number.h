#ifndef __kernel_number_h
#define __kernel_number_h

#include <kernel/def.h>
#include <kernel/ref.h>

struct number {
        struct ref base;
        double internal;
};

struct number *number_new();
struct number *number_init(struct number *p);

void number_release(struct number *p);

#define number_to(p, t) ((t)(p)->internal)

#define number_set(p, n) ((p)->internal += (n))

#endif
