#include <stdlib.h>
#include <stdarg.h>
#include <action/action_parallel.h>

static void __action_parallel_free(const struct ref *ref);
static void __action_parallel_update(struct action *a, float t, jmp_buf *env);
static void __action_parallel_reset(struct action *a);

struct action_parallel *action_parallel_new()
{
        return action_parallel_init(malloc(sizeof(struct action_parallel)));
}

struct action_parallel *action_parallel_init(struct action_parallel *p)
{
        action_init(&p->base);
        p->base.base = (struct ref){__action_parallel_free, 1};
        p->base.update = __action_parallel_update;
        p->base.reset = __action_parallel_reset;
        p->actions = array_new(1);
        p->bits = bits_new(1);
        return p;
}

void action_parallel_release(struct action_parallel *p)
{
        ref_dec(&p->actions->base);
        ref_dec(&p->bits->base);
}

static void __action_parallel_free(const struct ref *ref)
{
        struct action *a = cast(ref, struct action, base);
        struct action_parallel *p = cast(a, struct action_parallel, base);

        action_parallel_release(p);
        action_release(a);

        free(p);
}

static void __action_parallel_reset(struct action *a)
{
        unsigned i;
        struct action_parallel *p = cast(a, struct action_parallel, base);
        struct action *c;

        for(i = 0; i < p->actions->len; ++i) {
                c = cast(array_get(p->actions, i), struct action, base);
                action_reset(c);
        }
        bits_zero(p->bits);
}

static void __action_parallel_update(struct action *a, float t, jmp_buf *env)
{
        unsigned i, finish;
        struct action_parallel *p = cast(a, struct action_parallel, base);
        struct action *c;

        finish = 1;
        for(i = 0; i < p->actions->len; ++i) {
                c = cast(array_get(p->actions, i), struct action, base);
                if(bits_get(p->bits, i) == 0) {
                        bits_set(p->bits, 1, i);
                        action_reset(c);
                }
                action_update(c, t, env);
                finish &= c->finish;
        }
        a->finish = finish;
}

struct action *action_parallel(struct action *p, ...)
{
        struct action_parallel *s = action_parallel_new();
        struct action *c;

        va_list parg;
        va_start(parg, p);

        c = p;
        while(c) {
                bits_add(s->bits, 0);
                array_push(s->actions, &p->base);
                ref_dec(&p->base);
                c = va_arg(parg, struct action *);
        }
        va_end(parg);

        return &s->base;
}

struct action *action_parallel_repeat(signed step, struct action *p, ...)
{
        struct action_parallel *s = action_parallel_new();
        struct action *c;

        s->base.step = step;

        va_list parg;
        va_start(parg, p);

        c = p;
        while(c) {
                bits_add(s->bits, 0);
                array_push(s->actions, &p->base);
                ref_dec(&p->base);
                c = va_arg(parg, struct action *);
        }
        va_end(parg);

        return &s->base;
}
