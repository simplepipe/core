#include <stdlib.h>
#include <action/manager.h>

static void __action_manager_free(const struct ref *ref);

struct action_manager *action_manager_new()
{
        return action_manager_init(malloc(sizeof(struct action_manager)));
}

struct action_manager *action_manager_init(struct action_manager *p)
{
        p->base = (struct ref){__action_manager_free, 1};
        p->actions = array_new(0);
        p->bits = bits_new(0);
        return p;
}

void action_manager_release(struct action_manager *p)
{
        ref_dec(&p->actions->base);
        ref_dec(&p->bits->base);
}

static void __action_manager_free(const struct ref *ref)
{
        struct action_manager *p = cast(ref, struct action_manager, base);
        action_manager_release(p);
        free(p);
}

void action_manager_add_action(struct action_manager *p, struct action *a)
{
        bits_add(p->bits, 0);
        array_push(p->actions, &a->base);
        ref_dec(&a->base);
}

void action_manager_clear(struct action_manager *p)
{
        array_clear(p->actions);
        bits_clear(p->bits);
}

void action_manager_update(struct action_manager *p, float t)
{
        signed i, r;
        jmp_buf env;
        struct action * volatile a = NULL;

        ref_inc(&p->base);

        r = setjmp(env);
        if(r == 0) {
                for(i = 0; i < p->actions->len; ++i) {
                        a = cast(array_get(p->actions, i), struct action, base);
                        if(a->finish) {
                                array_remove(p->actions, i);
                                bits_remove(p->bits, i);
                                i--;
                        } else {
                                ref_inc(&a->base);
                                if(bits_get(p->bits, i) == 0) {
                                        bits_set(p->bits, 1, i);
                                        action_reset(a);
                                }
                                action_update(a, t, &env);
                                ref_dec(&a->base);

                                if(a->finish) {
                                        array_remove(p->actions, i);
                                        bits_remove(p->bits, i);
                                        i--;
                                }
                        }
                }
        } else {
                ref_dec(&a->base);

                for(i = 0; i < p->actions->len; ++i) {
                        a = cast(array_get(p->actions, i), struct action, base);
                        if(a->finish) {
                                array_remove(p->actions, i);
                                bits_remove(p->bits, i);
                                i--;
                        }
                }
        }

        ref_dec(&p->base);
}
