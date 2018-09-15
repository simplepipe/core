#include <stdlib.h>
#include <collada/context.h>

static void __dae_context_free(const struct ref *ref);

struct dae_context *dae_context_new()
{
        return dae_context_init(malloc(sizeof(struct dae_context)));
}

struct dae_context *dae_context_init(struct dae_context *p)
{
        p->base = (struct ref){__dae_context_free, 1};
        p->geometries = hash_table_new();
        p->skines = hash_table_new();
        p->amatures = hash_table_new();
        p->nodes = hash_table_new();
        return p;
}

void dae_context_release(struct dae_context *p)
{
#define DEC(v) \
        if(v) {\
                ref_dec(&v->base);\
                v = NULL;\
        }

        DEC(p->geometries)
        DEC(p->skines)
        DEC(p->amatures)
        DEC(p->nodes)

#undef DEC
}

static void __dae_context_free(const struct ref *ref)
{
        struct dae_context *p = cast(ref, struct dae_context, base);
        dae_context_release(p);
        free(p);
}
