#include <math/math.h>
#include <stdlib.h>
#include <action/action_ease.h>

static void __action_ease_by_free(const struct ref *ref);
static void __action_ease_by_update(struct action *a, float t, jmp_buf *env);
static void __action_ease_by_reset(struct action *a);


struct action_ease_by *action_ease_by_new(const unsigned len,
        float *target, float *amount, const float duration,
        float(*ease)(const float, const float))
{
        struct action_ease_by *p = action_ease_by_init(
                malloc(sizeof(struct action_ease_by)));

        p->target = target;
        *(unsigned *)&p->len = len;
        *(float *)&p->duration = duration;
        p->remain = duration;
        p->ease = ease;

        p->amount = malloc(sizeof(float) * len);
        p->offset = malloc(sizeof(float) * len);
        __set(len, p->amount, amount);
        __set_scalar(len, p->offset, 0.0f);

        p->base.update = __action_ease_by_update;
        p->base.reset = __action_ease_by_reset;

        return p;
}

struct action_ease_by *action_ease_by_init(struct action_ease_by *p)
{
        action_init(&p->base);
        p->base.base = (struct ref){__action_ease_by_free, 1};
        p->target = NULL;
        p->amount = NULL;
        p->offset = NULL;
        p->ease = NULL;
        return p;
}

void action_ease_by_release(struct action_ease_by *p)
{
        if(p->amount) {
                free(p->amount);
                p->amount = NULL;
        }
        if(p->offset) {
                free(p->offset);
                p->offset = NULL;
        }
}

static void __action_ease_by_free(const struct ref *ref)
{
        struct action *a = cast(ref, struct action, base);
        struct action_ease_by *p = cast(a, struct action_ease_by, base);

        action_ease_by_release(p);
        action_release(a);

        free(p);
}

static void __action_ease_by_update(struct action *a, float t, jmp_buf *env)
{
        float pc;
        struct action_ease_by *p = cast(a, struct action_ease_by, base);

        p->remain -= t;
        pc = p->ease(p->duration, p->remain);

        __sub(p->len, p->target, p->offset);
        __set(p->len, p->offset, p->amount);
        __mul_scalar(p->len, p->offset, pc);
        __add(p->len, p->target, p->offset);

        if(p->remain <= EPSILON) {
                p->base.finish = 1;
                p->remain = 0.0f;
        }
}

static void __action_ease_by_reset(struct action *a)
{
        struct action_ease_by *p = cast(a, struct action_ease_by, base);

        __set_scalar(p->len, p->offset, 0.0f);
        p->remain = p->duration;
}

/*
 * ease to
 */

static void __action_ease_to_free(const struct ref *ref);
static void __action_ease_to_reset(struct action *a);
static void __action_ease_to_update(struct action *a, float t, jmp_buf *env);

struct action_ease_to *action_ease_to_new(const unsigned len,
        float *target, float *dest, const float duration,
        float(*ease)(const float, const float))
{
        struct action_ease_to *p = action_ease_to_init(
                malloc(sizeof(struct action_ease_to)));

        p->base.target = target;
        *(unsigned *)&p->base.len = len;
        *(float *)&p->base.duration = duration;
        p->base.remain = duration;
        p->base.ease = ease;

        p->dest = malloc(sizeof(float) * len);
        __set(len, p->dest, dest);
        p->base.amount = malloc(sizeof(float) * len);
        p->base.offset = malloc(sizeof(float) * len);
        __set_scalar(len, p->base.offset, 0.0f);

        p->base.base.update = __action_ease_to_update;
        p->base.base.reset = __action_ease_to_reset;

        return p;
}

struct action_ease_to *action_ease_to_init(struct action_ease_to *p)
{
        action_ease_by_init(&p->base);
        p->base.base.base = (struct ref){__action_ease_to_free, 1};
        p->dest = NULL;
        return p;
}

void action_ease_to_release(struct action_ease_to *p)
{
        if(p->dest) {
                free(p->dest);
                p->dest = NULL;
        }
}

static void __action_ease_to_update(struct action *a, float t, jmp_buf *env)
{
        struct action_ease_by *b;
        struct action_ease_to *p;

        __action_ease_by_update(a, t, env);

        if(a->finish) {
                b = cast(a, struct action_ease_by, base);
                p = cast(b, struct action_ease_to, base);
                __set(b->len, b->target, p->dest);
        }
}

static void __action_ease_to_reset(struct action *a)
{
        struct action_ease_by *b = cast(a, struct action_ease_by, base);
        struct action_ease_to *p = cast(b, struct action_ease_to, base);

        __set_scalar(b->len, b->offset, 0.0f);
        b->remain = b->duration;

        __set(b->len, b->amount, p->dest);
        __sub(b->len, b->amount, b->target);
}

static void __action_ease_to_free(const struct ref *ref)
{
        struct action *a = cast(ref, struct action, base);
        struct action_ease_by *b = cast(a, struct action_ease_by, base);
        struct action_ease_to *p = cast(b, struct action_ease_to, base);

        action_ease_to_release(p);
        action_ease_by_release(b);
        action_release(a);

        free(p);
}
