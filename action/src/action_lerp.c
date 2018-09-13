#include <stdlib.h>
#include <math/math.h>
#include <action/action_lerp.h>

static void __action_lerp_free(const struct ref *ref);
static void __action_lerp_update(struct action *a, float t, jmp_buf *env);

struct action_lerp *action_lerp_new(const unsigned len,
        float *target, const float *dest, void(*free_dest)(const void*),
        const float rate, const float epsilon)
{
        return action_lerp_init(malloc(sizeof(struct action_lerp)),
                len, target, dest, free_dest, rate, epsilon);
}

struct action_lerp *action_lerp_init(struct action_lerp *p,
        const unsigned len,
        float *target, const float *dest, void(*free_dest)(const void*),
        const float rate, const float epsilon)
{
        action_init(&p->base);
        p->base.base = (struct ref){__action_lerp_free, 1};
        p->target = target;
        p->dest = dest;
        p->free_dest = free_dest;
        *(float *)&p->rate = rate;
        *(float *)&p->epsilon = epsilon;
        *(unsigned *)&p->len = len;

        p->base.update = __action_lerp_update;
        p->base.reset = NULL;

        return p;
}

void action_lerp_release(struct action_lerp *p)
{
        if(p->free_dest && p->dest) {
                p->free_dest(p->dest);
                p->free_dest = NULL;
                p->dest = NULL;
        }
}

static void __action_lerp_free(const struct ref *ref)
{
        struct action * const a = cast(ref, struct action, base);
        struct action_lerp * const p = cast(a, struct action_lerp, base);

        action_lerp_release(p);
        action_release(a);

        free(p);
}

static void __action_lerp_update(struct action *a, float t, jmp_buf *env)
{
        float d;
        struct action_lerp * const p = cast(a, struct action_lerp, base);
        const float * const restrict dest = p->dest;

        __lerp(p->len, p->target, dest, p->rate * t);
        d = __offset(p->len, p->target, dest);

        if(fabs(d) <= p->epsilon) {
                __set(p->len, p->target, dest);
                p->base.finish = 1;
        }
}
