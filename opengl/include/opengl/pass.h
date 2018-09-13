#ifndef __opengl_pass_h
#define __opengl_pass_h

#include <kernel/ref.h>
#include <kernel/array.h>
#include <opengl/config.h>
#include <opengl/texture.h>

struct pass {
        struct ref base;

        unsigned id_sampled;
        unsigned id_resolved;

        unsigned id_depth_stencil;

        unsigned msaa : 1;

        unsigned width;
        unsigned height;

        void(*begin)(struct pass *);
        void(*end)(struct pass *);
};

struct pass *pass_main_new();
struct pass *pass_init(struct pass *p);
void pass_release(struct pass *p);

struct pass_texture {
        struct pass base;

        struct array *textures;
};
struct pass_texture *pass_texture_new(const unsigned len,
        const unsigned width, const unsigned height,
        const unsigned depth, const unsigned stencil);
struct pass_texture *pass_texture_new_msaa(const unsigned len,
        const unsigned width, const unsigned height,
        const unsigned depth, const unsigned stencil);
struct pass_texture *pass_texture_init(struct pass_texture *p,
        const unsigned len,
        const unsigned width, const unsigned height,
        const unsigned depth, const unsigned stencil,
        const unsigned msaa);
void pass_texture_release(struct pass_texture *p);

#endif
