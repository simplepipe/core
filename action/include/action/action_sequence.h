#ifndef __action_sequence_h
#define __action_sequence_h

#include <kernel/array.h>
#include <kernel/bits.h>
#include <action/action.h>

struct action_sequence {
        struct action base;

        struct array *actions;
        struct bits *bits;
        unsigned cid;
};

struct action_sequence *action_sequence_new();
struct action_sequence *action_sequence_init(struct action_sequence *p);
void action_sequence_release(struct action_sequence *p);

struct action *action_sequence(struct action *p, ...);
struct action *action_sequence_repeat(signed step, struct action *p, ...);

#endif
