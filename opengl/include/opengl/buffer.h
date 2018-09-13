#ifndef __opengl_buffer_h
#define __opengl_buffer_h

#include <kernel/ref.h>
#include <kernel/array.h>
#include <opengl/config.h>

struct vbo {
        struct ref base;

        unsigned id;
};

struct vbo *vbo_new();
struct vbo *vbo_init(struct vbo *p);
void vbo_release(struct vbo *p);

struct vao {
        struct ref base;

        unsigned id;
        struct array *vbos;
};

struct vao *vao_new();
struct vao *vao_init(struct vao *p);
void vao_release(struct vao *p);
void vao_add_vbo(struct vao *p, struct vbo *b);

#endif
