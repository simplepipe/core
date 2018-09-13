#ifndef __action_manager_h
#define __action_manager_h

#include <kernel/array.h>
#include <kernel/bits.h>
#include <action/action.h>

struct action_manager {
        struct ref base;

        struct array *actions;
        struct bits *bits;
};

struct action_manager *action_manager_new();
struct action_manager *action_manager_init(struct action_manager *p);
void action_manager_release(struct action_manager *p);

void action_manager_add_action(struct action_manager *p, struct action *a);

void action_manager_update(struct action_manager *p, float t);
void action_manager_clear(struct action_manager *p);

#endif
