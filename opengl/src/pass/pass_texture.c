#include <stdlib.h>
#include <opengl/pass.h>

static void __pass_texture_free(const struct ref *ref);

struct pass_texture *pass_texture_new(const unsigned len,
        const unsigned width, const unsigned height,
        const unsigned depth, const unsigned stencil)
{
        unsigned i;
        struct texture *t;
        unsigned attachments[32];

        struct pass_texture *p = pass_texture_init(
                malloc(sizeof(struct pass_texture)),
                len, width, height, depth, stencil, 0
        );

        glGenFramebuffers(1, &p->base.id_resolved);
        glBindFramebuffer(GL_FRAMEBUFFER, p->base.id_resolved);
        if(depth || stencil) {
                glGenRenderbuffers(1, &p->base.id_depth_stencil);
                glBindRenderbuffer(GL_RENDERBUFFER, p->base.id_depth_stencil);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
                if(depth) glFramebufferRenderbuffer(
                        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                        GL_RENDERBUFFER, p->base.id_depth_stencil);
                if(stencil) glFramebufferRenderbuffer(
                        GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                        GL_RENDERBUFFER, p->base.id_depth_stencil);
        }

        for(i = 0; i < len; ++i) {
                t = texture_new();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, t->id);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width,
                        height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                        GL_TEXTURE_2D, t->id, 0);

                array_push(p->textures, &t->base);
                ref_dec(&t->base);
                attachments[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        glDrawBuffers(len, attachments);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                pass_texture_release(p);
                pass_release(&p->base);
        }

        return p;
}

struct pass_texture *pass_texture_init(struct pass_texture *p,
        const unsigned len,
        const unsigned width, const unsigned height,
        const unsigned depth, const unsigned stencil,
        const unsigned msaa)
{
        pass_init(&p->base);
        p->base.base = (struct ref){__pass_texture_free, 1};
        p->base.width = width;
        p->base.height = height;
        p->base.msaa = msaa;
        p->textures = array_new(1);

        return p;
}

void pass_texture_release(struct pass_texture *p)
{
        ref_dec(&p->textures->base);
}

static void __pass_texture_free(const struct ref *ref)
{
        struct pass *p = cast(ref, struct pass, base);
        struct pass_texture *pt = cast(p, struct pass_texture, base);

        pass_texture_release(pt);
        pass_release(p);

        free(pt);
}
