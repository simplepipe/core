#include <stdlib.h>
#include <string.h>
#include <opengl/buffer.h>

static void __vbo_free(const struct ref *ref);

struct vbo *vbo_new()
{
        return vbo_init(malloc(sizeof(struct vbo)));
}

struct vbo *vbo_init(struct vbo *p)
{
        p->base = (struct ref){__vbo_free, 1};
        glGenBuffers(1, &p->id);
        return p;
}

void vbo_release(struct vbo *p)
{
        if(p->id > 0) {
                glDeleteBuffers(1, &p->id);
                p->id = 0;
        }
}

static void __vbo_free(const struct ref *ref)
{
        struct vbo *p = cast(ref, struct vbo, base);
        vbo_release(p);
        free(p);
}
