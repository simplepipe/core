#include <stdlib.h>
#include <opengl/pass.h>
#include <opengl/device.h>

static void __pass_main_free(const struct ref *ref);

struct pass *pass_main_new()
{
        struct pass *p = pass_init(malloc(sizeof(struct pass)));
        p->base = (struct ref){__pass_main_free, 1};
        p->id_sampled = g_device.id_sampled;
        p->id_resolved = g_device.id_resolved;
        if(p->id_sampled) {
                p->msaa = 1;
        } else {
                p->msaa = 0;
        }
        p->width = g_device.width;
        p->height = g_device.height;
        return p;
}

static void __pass_main_free(const struct ref *ref)
{
        struct pass *p = cast(ref, struct pass, base);
        pass_release(p);
        free(p);
}
