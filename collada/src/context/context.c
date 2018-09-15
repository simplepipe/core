#include <stdlib.h>
#include <collada/context.h>

static void __collada_context_free(const struct ref *ref);

struct collada_context *collada_context_new()
{
        return collada_context_init(malloc(sizeof(struct collada_context)));
}

struct collada_context *collada_context_init(struct collada_context *p)
{
        p->base = (struct ref){__collada_context_free, 1};
        p->geometries = hash_table_new();
        p->skines = hash_table_new();
        p->amatures = hash_table_new();
        p->nodes = hash_table_new();
        return p;
}

void collada_context_release(struct collada_context *p)
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

static void __collada_context_free(const struct ref *ref)
{
        struct collada_context *p = cast(ref, struct collada_context, base);
        collada_context_release(p);
        free(p);
}
