#ifndef __opengl_texture_h
#define __opengl_texture_h

#include <kernel/ref.h>
#include <opengl/config.h>

struct texture {
        struct ref base;

        unsigned id;
        unsigned width;
        unsigned height;
};

struct texture *texture_new();
struct texture *texture_init(struct texture *p);
void texture_release(struct texture *p);

#endif
