#include <stdlib.h>
#include <collada/amature.h>

static void __dae_amature_free(const struct ref *ref);

struct dae_amature *dae_amature_new()
{
        return dae_amature_init(malloc(sizeof(struct dae_amature)));
}

struct dae_amature *dae_amature_init(struct dae_amature *p)
{
        p->base = (struct ref){__dae_amature_free, 1};
        mat4_set_identity(&p->transform);
        p->bones = hash_table_new();
        p->id = string_new();
        return p;
}

void dae_amature_release(struct dae_amature *p)
{
        ref_dec(&p->bones->base);
        ref_dec(&p->id->base);
}

static void __dae_amature_free(const struct ref *ref)
{
        struct dae_amature *p = cast(ref, struct dae_amature, base);
        dae_amature_release(p);
        free(p);
}

void dae_amature_add_bone(struct dae_amature *p, struct dae_bone *b)
{
        hash_table_set(p->bones, &b->base, b->id->ptr, b->id->len);
        hash_table_set(p->bones, &b->base, b->sid->ptr, b->sid->len);
}

void dae_amature_assign(struct dae_amature **p, struct dae_amature *a)
{
        if(a) {
                ref_inc(&a->base);
        }
        if(*p) {
                ref_dec(&(*p)->base);
        }
        *p = a;
}
