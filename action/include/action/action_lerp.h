#ifndef __action_lerp_h
#define __action_lerp_h

#include <action/action.h>

struct action_lerp {
        struct action base;

        const unsigned len;
        float *target;
        const float *dest;
        void(*free_dest)(const void *);
        const float rate;
        const float epsilon;
};

struct action_lerp *action_lerp_new(const unsigned len,
        float *target, const float *dest, void(*free_dest)(const void*),
        const float rate, const float epsilon);
struct action_lerp *action_lerp_init(struct action_lerp *p,
        const unsigned len,
        float *target, const float *dest, void(*free_dest)(const void*),
        const float rate, const float epsilon);
void action_lerp_release(struct action_lerp *p);

#endif
