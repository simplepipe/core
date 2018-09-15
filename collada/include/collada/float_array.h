#ifndef __float_array_h
#define __float_array_h

#include <kernel/ref.h>

struct float_array {
        struct ref base;

        float * ptr;
        unsigned len;
        unsigned reserved;
};

struct float_array *float_array_new();
struct float_array *float_array_init(struct float_array *p);
void float_array_release(struct float_array *p);
void float_array_reserve(struct float_array *p, const unsigned len);
void float_array_add_string(struct float_array *p, const char *buf);
void float_array_add_stream(struct float_array *p, const float * restrict buf,
        const unsigned len);
void float_array_add_float(struct float_array *p, const float f);

void float_array_assign(struct float_array **p, struct float_array *a);

#endif
