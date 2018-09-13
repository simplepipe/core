#ifndef __kernel_ref_h
#define __kernel_ref_h

#include <stddef.h>
#include <kernel/def.h>

struct ref {
        void(*free)(const struct ref *);
        volatile signed count;
};

static inline void
ref_inc(const struct ref *ref)
{
        __sync_add_and_fetch((volatile signed *)&ref->count, 1);
}

static inline void
ref_dec(const struct ref *ref)
{
        if (__sync_sub_and_fetch((volatile signed *)&ref->count, 1) == 0) {
                ref->free(ref);
        }
}

#define cast(ptr, type, member) \
    ((type *) ((char *)(ptr) - offsetof(type, member)))

#define cast_null(ptr, type, member) \
        ((type *) (ptr ? ((char *)(ptr) - offsetof(type, member)) : NULL))

#endif
