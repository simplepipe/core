#ifndef __kernel_bits_h
#define __kernel_bits_h

#include <kernel/ref.h>

struct bits {
        struct ref base;

        unsigned *ptr;
        unsigned len;
        unsigned reserved;
        unsigned ordered : 1;
};

struct bits *bits_new(const unsigned ordered);
struct bits *bits_init(struct bits *p, const unsigned ordered);
void bits_release(struct bits *p);

void bits_add(struct bits *p, unsigned b);
void bits_set(struct bits *p, unsigned b, unsigned id);
unsigned bits_get(struct bits *p, unsigned id);
void bits_zero(struct bits *p);
void bits_remove(struct bits *p, unsigned id);
void bits_clear(struct bits *p);

#endif
