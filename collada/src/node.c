#include <stdlib.h>
#include <collada/node.h>

static void __dae_node_free(const struct ref *ref);

struct dae_node *dae_node_new()
{
        return dae_node_init(malloc(sizeof(struct dae_node)));
}

struct dae_node *dae_node_init(struct dae_node *p)
{
        p->base = (struct ref){__dae_node_free, 1};
        p->id = string_new();
        p->children = array_new(1);
        p->parent = NULL;

        p->amature = NULL;
        p->vertices = NULL;
        p->normals = NULL;
        p->texcoords = NULL;
        p->colors = NULL;
        p->bones_id = NULL;
        p->weights = NULL;
        p->inverse_bind_poses = NULL;
        p->bone_names = NULL;
        p->bones_per_vertex = 0;
        p->bones_upload = 0;
        mat4_set_identity(&p->transform);
        return p;
}

void dae_node_release(struct dae_node *p)
{
#define DEC(v) \
        if(v) {\
                ref_dec(&v->base);\
                v = NULL;\
        }

        DEC(p->id);
        DEC(p->children);
        DEC(p->amature);
        DEC(p->vertices);
        DEC(p->normals);
        DEC(p->texcoords);
        DEC(p->colors);
        DEC(p->bones_id);
        DEC(p->weights);
        DEC(p->bone_names);
        DEC(p->inverse_bind_poses);

#undef DEC
}

static void __dae_node_free(const struct ref *ref)
{
        struct dae_node *p = cast(ref, struct dae_node, base);
        dae_node_release(p);
        free(p);
}

void dae_node_add_child(struct dae_node *p, struct dae_node *c)
{
        array_push(p->children, &c->base);
        c->parent = p;
}
