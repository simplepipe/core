#ifndef __action_action_h
#define __action_action_h
#include <setjmp.h>
#include <kernel/ref.h>
#include <kernel/list.h>
#include <action/ease.h>

struct action {
        struct ref base;

        void(*update)(struct action *, float, jmp_buf *env);
        void(*reset)(struct action *);
        void(*callback)(const struct ref *);

        struct ref *data;

        unsigned finish : 1;
        signed step;
};

struct action *action_new();
struct action *action_init(struct action *p);
void action_release(struct action *p);

struct action *action_set_callback(struct action *p,
        void(*callback)(const struct ref *), struct ref *data);

void action_update(struct action *p, float t, jmp_buf *env);
void action_reset(struct action *p);

#endif
