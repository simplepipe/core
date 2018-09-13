#include <stdlib.h>
#include <string.h>
#include <opengl/buffer.h>

static void __vao_free(const struct ref *ref);

struct vao *vao_new()
{
        return vao_init(malloc(sizeof(struct vao)));
}

struct vao *vao_init(struct vao *p)
{
        p->base = (struct ref){__vao_free, 1};
        glGenVertexArrays(1, &p->id);
        p->vbos = array_new(1);
        return p;
}

void vao_release(struct vao *p)
{
        ref_dec(&p->vbos->base);
        if(p->id > 0) {
                glDeleteVertexArrays(1, &p->id);
                p->id = 0;
        }
}

static void __vao_free(const struct ref *ref)
{
        struct vao *p = cast(ref, struct vao, base);
        vao_release(p);
        free(p);
}

void vao_add_vbo(struct vao *p, struct vbo *b)
{
        array_push(p->vbos, &b->base);
}
