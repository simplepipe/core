#include <opengl/pass.h>

static void __pass_begin(struct pass *p);
static void __pass_end(struct pass *p);

struct pass *pass_init(struct pass *p)
{
        p->base = (struct ref){NULL, 1};
        p->id_sampled = 0;
        p->id_resolved = 0;
        p->id_depth_stencil = 0;
        p->begin = __pass_begin;
        p->end = __pass_end;
        p->msaa = 0;
        p->width = 0;
        p->height = 0;
        return p;
}

void pass_release(struct pass *p)
{
        if(p->id_sampled > 0) {
                glDeleteFramebuffers(1, &p->id_sampled);
                p->id_sampled = 0;
        }
        if(p->id_resolved > 0) {
                glDeleteFramebuffers(1, &p->id_resolved);
                p->id_resolved = 0;
        }
        if(p->id_depth_stencil) {
                glDeleteRenderbuffers(1, &p->id_depth_stencil);
                p->id_depth_stencil = 0;
        }
}

static void __pass_begin(struct pass *p)
{
        if(p->msaa && p->id_sampled > 0) {
                glBindFramebuffer(GL_FRAMEBUFFER, p->id_sampled);
        } else if(p->id_resolved > 0) {
                glBindFramebuffer(GL_FRAMEBUFFER, p->id_resolved);
        }
        glViewport(0, 0, p->width, p->height);
}

static void __pass_end(struct pass *p)
{
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
