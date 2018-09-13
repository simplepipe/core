#ifndef __opengl_shader_h
#define __opengl_shader_h

#include <kernel/ref.h>
#include <opengl/config.h>

struct shader {
        struct ref base;

        unsigned id;
};

struct shader *shader_new(const char *vert, const char *frag);
struct shader *shader_init(struct shader *p,
        const char *vert, const char *frag);
void shader_release(struct shader *p);

#endif
