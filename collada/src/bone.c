#include <stdlib.h>
#include <collada/bone.h>

static void __dae_bone_free(const struct ref *ref);

struct dae_bone *dae_bone_new()
{
        return dae_bone_init(malloc(sizeof(struct dae_bone)));
}

struct dae_bone *dae_bone_init(struct dae_bone *p)
{
        p->base = (struct ref){__dae_bone_free, 1};
        p->children = array_new(1);
        p->parent = NULL;
        p->id = string_new();
        p->sid = string_new();
        mat4_set_identity(&p->transform);
        mat4_set_identity(&p->transform_relative);
        return p;
}

void dae_bone_release(struct dae_bone *p)
{
        ref_dec(&p->children->base);
        ref_dec(&p->id->base);
        ref_dec(&p->sid->base);
}

static void __dae_bone_free(const struct ref *ref)
{
        struct dae_bone *p = cast(ref, struct dae_bone, base);
        dae_bone_release(p);
        free(p);
}

void dae_bone_add_bone(struct dae_bone *p, struct dae_bone *c)
{
        array_push(p->children, &c->base);
        c->parent = p;
}
