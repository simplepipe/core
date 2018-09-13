#include <stdlib.h>
#include <stdarg.h>
#include <action/action_sequence.h>

static void __action_sequence_free(const struct ref *ref);
static void __action_sequence_update(struct action *a, float t, jmp_buf *env);
static void __action_sequence_reset(struct action *a);

struct action_sequence *action_sequence_new()
{
        return action_sequence_init(malloc(sizeof(struct action_sequence)));
}

struct action_sequence *action_sequence_init(struct action_sequence *p)
{
        action_init(&p->base);
        p->base.base = (struct ref){__action_sequence_free, 1};
        p->base.update = __action_sequence_update;
        p->base.reset = __action_sequence_reset;
        p->actions = array_new(1);
        p->bits = bits_new(1);
        p->cid = 0;
        return p;
}

void action_sequence_release(struct action_sequence *p)
{
        ref_dec(&p->actions->base);
        ref_dec(&p->bits->base);
}

static void __action_sequence_free(const struct ref *ref)
{
        struct action *a = cast(ref, struct action, base);
        struct action_sequence *p = cast(a, struct action_sequence, base);

        action_sequence_release(p);
        action_release(a);

        free(p);
}

static void __action_sequence_reset(struct action *a)
{
        unsigned i;
        struct action_sequence *p = cast(a, struct action_sequence, base);
        struct action *c;

        p->cid = 0;
        for(i = 0; i < p->actions->len; ++i) {
                c = cast(array_get(p->actions, i), struct action, base);
                action_reset(c);
        }
        bits_zero(p->bits);
}

static void __action_sequence_update(struct action *a, float t, jmp_buf *env)
{
        struct action_sequence *p = cast(a, struct action_sequence, base);
        struct action *c;

        if(p->cid < p->actions->len) {
                c = cast(array_get(p->actions, p->cid), struct action, base);
                if(bits_get(p->bits, p->cid) == 0) {
                        bits_set(p->bits, 1, p->cid);
                        action_reset(c);
                }
                action_update(c, t, env);
                if(c->finish) {
                        p->cid++;
                }
        }
        if(p->cid == p->actions->len) {
                a->finish = 1;
        }
}

struct action *action_sequence(struct action *p, ...)
{
        struct action_sequence *s = action_sequence_new();
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

struct action *action_sequence_repeat(signed step, struct action *p, ...)
{
        struct action_sequence *s = action_sequence_new();
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
