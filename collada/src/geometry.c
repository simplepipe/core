#include <stdlib.h>
#include <collada/geometry.h>

static void __dae_geometry_free(const struct ref *ref);

struct dae_geometry *dae_geometry_new()
{
        return dae_geometry_init(malloc(sizeof(struct dae_geometry)));
}

struct dae_geometry *dae_geometry_init(struct dae_geometry *p)
{
        p->base = (struct ref){__dae_geometry_free, 1};
        p->vertices = NULL;
        p->normals = NULL;
        p->texcoords = NULL;
        p->colors = NULL;
        p->id = string_new();
        return p;
}

void dae_geometry_release(struct dae_geometry *p)
{
#define DEC(v) \
        if(v) {\
                ref_dec(&v->base);\
                v = NULL;\
        }

        DEC(p->id);
        DEC(p->vertices);
        DEC(p->normals);
        DEC(p->texcoords);
        DEC(p->colors);

#undef DEC
}

static void __dae_geometry_free(const struct ref *ref)
{
        struct dae_geometry *p = cast(ref, struct dae_geometry, base);
        dae_geometry_release(p);
        free(p);
}
