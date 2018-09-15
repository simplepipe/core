#ifndef __collada_bone_h
#define __collada_bone_h

#include <math/math.h>
#include <kernel/array.h>
#include <kernel/string.h>

struct dae_bone {
        struct ref base;

        struct array *children;
        struct dae_bone *parent;

        union mat4 transform;
        union mat4 transform_relative;

        struct string *id;
        struct string *sid;

        struct float_array *animation_times;
        struct float_array *animation_transforms;
};

struct dae_bone *dae_bone_new();
struct dae_bone *dae_bone_init(struct dae_bone *p);
void dae_bone_release(struct dae_bone *p);

void dae_bone_add_bone(struct dae_bone *p, struct dae_bone *c);

#endif
