#ifndef __action_ease_by_h
#define __action_ease_by_h

#include <action/action.h>
#include <action/ease.h>

struct action_ease_by {
        struct action base;

        const unsigned len;
        float *target;
        float *amount;
        float *offset;

        float(*ease)(const float, const float);

        const float duration;
        float remain;
};

struct action_ease_by *action_ease_by_new(const unsigned len,
        float *target, float *amount, const float duration,
        float(*ease)(const float, const float));
struct action_ease_by *action_ease_by_init(struct action_ease_by *p);
void action_ease_by_release(struct action_ease_by *p);

struct action_ease_to {
        struct action_ease_by base;

        float *dest;
};

struct action_ease_to *action_ease_to_new(const unsigned len,
        float *target, float *dest, const float duration,
        float(*ease)(const float, const float));
struct action_ease_to *action_ease_to_init(struct action_ease_to *p);
void action_ease_to_release(struct action_ease_to *p);

#endif
