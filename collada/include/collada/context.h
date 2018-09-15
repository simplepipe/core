#ifndef __collada_context_h
#define __collada_context_h

#include <kernel/ref.h>

struct collada_context {
        struct ref base;

        struct hash_table *float_arrays;
};

#endif
