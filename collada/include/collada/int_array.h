#ifndef __int_array_h
#define __int_array_h

#include <kernel/ref.h>

struct int_array {
        struct ref base;

        signed * ptr;
        unsigned len;
        unsigned reserved;
};

struct int_array *int_array_new();
struct int_array *int_array_init(struct int_array *p);
void int_array_release(struct int_array *p);
void int_array_reserve(struct int_array *p, const unsigned len);
signed int_array_add_string(struct int_array *p, const char *buf);
void int_array_add_stream(struct int_array *p, const signed * restrict buf,
        const unsigned len);
void int_array_add_int(struct int_array *p, const signed f);

void int_array_assign(struct int_array **p, struct int_array *a);

#endif
