#include <stdlib.h>
#include <collada/skin.h>

static void __dae_skin_free(const struct ref *ref);

struct dae_skin *dae_skin_new()
{
        return dae_skin_init(malloc(sizeof(struct dae_skin)));
}

struct dae_skin *dae_skin_init(struct dae_skin *p)
{
        p->base = (struct ref){__dae_skin_free, 1};
        p->vertices = NULL;
        p->normals = NULL;
        p->texcoords = NULL;
        p->colors = NULL;
        p->bones_id = float_array_new();
        p->weights = float_array_new();
        p->inverse_bind_poses = float_array_new();
        p->bone_names = array_new(1);
        p->id = string_new();
        p->bones_per_vertex = 0;
        p->bones_upload = 0;
        mat4_set_identity(&p->transform);
        return p;
}

void dae_skin_release(struct dae_skin *p)
{
#define DEC(v) \
        if(v) {\
                ref_dec(&v->base);\
                v = NULL;\
        }

        DEC(p->vertices);
        DEC(p->normals);
        DEC(p->texcoords);
        DEC(p->colors);
        DEC(p->bones_id);
        DEC(p->weights);
        DEC(p->bone_names);
        DEC(p->inverse_bind_poses);
        DEC(p->id);

#undef DEC
}

static void __dae_skin_free(const struct ref *ref)
{
        struct dae_skin *p = cast(ref, struct dae_skin, base);
        dae_skin_release(p);
        free(p);
}
