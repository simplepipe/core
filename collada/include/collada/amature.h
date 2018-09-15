#ifndef __collada_amature_h
#define __collada_amature_h

#include <math/math.h>
#include <kernel/hash_table.h>
#include <kernel/string.h>
#include <collada/bone.h>

struct dae_amature {
        struct ref base;

        union mat4 transform;

        struct hash_table *bones;

        struct string *name;
};

struct dae_amature *dae_amature_new();
struct dae_amature *dae_amature_init(struct dae_amature *p);
void dae_amature_release(struct dae_amature *p);

void dae_amature_add_bone(struct dae_amature *p, struct dae_bone *b);

void dae_amature_assign(struct dae_amature **p, struct dae_amature *a);

#endif
