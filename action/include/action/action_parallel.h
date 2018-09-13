#ifndef __action_parallel_h
#define __action_parallel_h

#include <kernel/array.h>
#include <kernel/bits.h>
#include <action/action.h>

struct action_parallel {
        struct action base;

        struct array *actions;
        struct bits *bits;
};

struct action_parallel *action_parallel_new();
struct action_parallel *action_parallel_init(struct action_parallel *p);
void action_parallel_release(struct action_parallel *p);

struct action *action_parallel(struct action *p, ...);
struct action *action_parallel_repeat(signed step, struct action *p, ...);

#endif
