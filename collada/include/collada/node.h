#ifndef __collada_node_h
#define __collada_node_h

#include <kernel/array.h>
#include <kernel/string.h>
#include <collada/float_array.h>
#include <collada/amature.h>

struct dae_node {
        struct ref base;

        struct string *id;

        struct array *children;
        struct dae_node *parent;

        struct dae_amature *amature;

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
};

struct dae_node *dae_node_new();
struct dae_node *dae_node_init(struct dae_node *p);
void dae_node_release(struct dae_node *p);
void dae_node_add_child(struct dae_node *p, struct dae_node *c);

#endif
