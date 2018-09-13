#include <stdlib.h>
#include <opengl/texture.h>

static void __texture_free(const struct ref *ref);

struct texture *texture_new()
{
        return texture_init(malloc(sizeof(struct texture)));
}

struct texture *texture_init(struct texture *p)
{
        p->base = (struct ref){__texture_free, 1};
        glGenTextures(1, &p->id);
        p->width = 0;
        p->height = 0;
        return p;
}

void texture_release(struct texture *p)
{
        if(p->id > 0) {
                glDeleteTextures(1, &p->id);
        }
}

static void __texture_free(const struct ref *ref)
{
        struct texture *p = cast(ref, struct texture, base);
        texture_release(p);
        free(p);
}
