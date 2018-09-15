#ifndef __collada_skin_h
#define __collada_skin_h

#include <math/math.h>
#include <kernel/string.h>
#include <kernel/array.h>
#include <collada/float_array.h>

struct dae_skin {
        struct ref base;

        struct float_array *vertices;
        struct float_array *normals;
        struct float_array *texcoords;
        struct float_array *colors;
        struct float_array *bones_id;
        struct float_array *weights;

        struct float_array *inverse_bind_poses;
        struct array *bone_names;

        unsigned bones_per_vertex;
        unsigned bones_upload;

        union mat4 transform;

        struct string *id;
};

struct dae_skin *dae_skin_new();
struct dae_skin *dae_skin_init(struct dae_skin *p);
void dae_skin_release(struct dae_skin *p);

#endif
