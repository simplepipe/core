#include <stdlib.h>
#include <action/action.h>

static void __action_free(const struct ref *ref);

struct action *action_new()
{
        return action_init(malloc(sizeof(struct action)));
}

struct action *action_init(struct action *p)
{
        p->base = (struct ref){__action_free, 1};
        p->data = NULL;
        p->finish = 0;
        p->step = 0;
        p->callback = NULL;
        p->reset = NULL;
        p->update = NULL;
        return p;
}

void action_release(struct action *p)
{
        if(p->data) {
                ref_dec(p->data);
                p->data = NULL;
        }
}

static void __action_free(const struct ref *ref)
{
        struct action *p = cast(ref, struct action, base);
        action_release(p);
        free(p);
}

struct action *action_set_callback(struct action *p,
        void(*callback)(const struct ref *), struct ref *data)
{
        action_release(p);

        p->callback = callback;
        p->data = data;
        if(data) {
                ref_inc(data);
        }

        return p;
}

void action_update(struct action *p, float t, jmp_buf *env)
{
        signed count;

        if(!p->finish) {
                ref_inc(&p->base);
                count = p->base.count;

                if(p->update) {
                        p->update(p, t, env);
                }
                if(p->callback) {
                        p->callback(p->data);
                }
                if(p->finish) {
                        if(p->step < 0) {
                                action_reset(p);
                        } else {
                                if(p->step > 0) {
                                        p->step--;
                                        action_reset(p);
                                }
                        }
                }

                if(count > p->base.count) {
                        p->finish = 1;
                        ref_dec(&p->base);
                        longjmp(*env, 1);
                } else {
                        ref_dec(&p->base);
                }
        }
}

void action_reset(struct action *p)
{
        p->finish = 0;
        if(p->reset) {
                p->reset(p);
        }
}
