#include <kernel/platform.h>

#if OS == IOS
#include <stdlib.h>
#include <opengl/pass.h>

struct pass_texture_ios {
        struct pass_texture base;

        unsigned *render_buffers_sampled;
        unsigned render_buffers_len;
};

static void __pass_texture_ios_free(const struct ref *ref);
static struct pass_texture_ios *__pass_texture_ios_init(
        struct pass_texture_ios *p, const unsigned len,
        const unsigned width, const unsigned height,
        const unsigned depth, const unsigned stencil,
        const unsigned msaa);
static void __pass_texture_ios_release(struct pass_texture_ios *p);
static void __pass_texture_ios_end(struct pass *p);

struct pass_texture *pass_texture_new_msaa(const unsigned len,
        const unsigned width, const unsigned height,
        const unsigned depth,const  unsigned stencil)
{
        unsigned i;
        struct texture *t;
        unsigned attachments[32];

        struct pass *p;
        struct pass_texture *pt;
        struct pass_texture_ios *pl;

        signed samples;
        glGetIntegerv(GL_MAX_SAMPLES, &samples);
        if(samples == 0) {
                pt = pass_texture_new(len, width, height, depth, stencil);
        } else {
                pl = __pass_texture_ios_init(
                        malloc(sizeof(struct pass_texture_ios)),
                        len, width, height, depth, stencil, 1);
                pt = &pl->base;
                p = &pt->base;

                glGenFramebuffers(1, &p->id_sampled);
                glBindFramebuffer(GL_FRAMEBUFFER, p->id_sampled);
                if(depth || stencil) {
                        glGenRenderbuffers(1, &p->id_depth_stencil);
                        glBindRenderbuffer(GL_RENDERBUFFER, p->id_depth_stencil);
                        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
                                GL_DEPTH24_STENCIL8, width, height);
                        if(depth) glFramebufferRenderbuffer(
                                GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                GL_RENDERBUFFER, p->id_depth_stencil);
                        if(stencil) glFramebufferRenderbuffer(
                                GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
                                GL_RENDERBUFFER, p->id_depth_stencil);
                }

                for(i = 0; i < len; ++i) {
                        glGenRenderbuffers(1, &pl->render_buffers_sampled[i]);
                        glBindRenderbuffer(GL_RENDERBUFFER, pl->render_buffers_sampled[i]);
                        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, width, height);
                        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                GL_RENDERBUFFER, pl->render_buffers_sampled[i]);

                        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
                }
                glDrawBuffers(len, attachments);

                if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                        __pass_texture_ios_release(pl);
                        pass_texture_release(pt);
                        pass_release(p);
                        goto finish;
                }

                glGenFramebuffers(1, &p->id_resolved);
                glBindFramebuffer(GL_FRAMEBUFFER, p->id_resolved);
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

                        array_push(pt->textures, &t->base);
                        ref_dec(&t->base);
                        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
                }
                glDrawBuffers(len, attachments);

                if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                        __pass_texture_ios_release(pl);
                        pass_texture_release(pt);
                        pass_release(p);
                        goto finish;
                }
        }

finish:
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return pt;
}

struct pass_texture_ios *__pass_texture_ios_init(
        struct pass_texture_ios *p, const unsigned len,
        const unsigned width, const unsigned height,
        const unsigned depth, const unsigned stencil,
        const unsigned msaa)
{
        pass_texture_init(&p->base, len, width, height, depth, stencil, msaa);
        p->base.base.base = (struct ref){__pass_texture_ios_free, 1};
        p->render_buffers_sampled = calloc(len, sizeof(unsigned));
        p->render_buffers_len = len;
        p->base.base.end = __pass_texture_ios_end;
        return p;
}

void __pass_texture_ios_release(struct pass_texture_ios *p)
{
        if(p->render_buffers_sampled) {
                glDeleteRenderbuffers(p->render_buffers_len,
                        p->render_buffers_sampled);
                free(p->render_buffers_sampled);
                p->render_buffers_sampled = NULL;
        }
}

static void __pass_texture_ios_free(const struct ref *ref)
{
        struct pass *p = cast(ref, struct pass, base);
        struct pass_texture *pt = cast(p, struct pass_texture, base);
        struct pass_texture_ios *ptl =
                cast(pt, struct pass_texture_ios, base);

        __pass_texture_ios_release(ptl);
        pass_texture_release(pt);
        pass_release(p);

        free(ptl);
}

static void __pass_texture_ios_end(struct pass *p)
{
        unsigned i;
        struct pass_texture *pt;

        pt = cast(p, struct pass_texture, base);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, p->id_sampled);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, p->id_resolved);
        for(i = 0 ; i < pt->textures->len; i++)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
		glBlitFramebuffer(
                        0, 0, p->width, p->height,
                        0, 0, p->width, p->height,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

#endif
