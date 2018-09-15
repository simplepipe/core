#ifndef __collada_geometry_h
#define __collada_geometry_h

#include <kernel/string.h>
#include <collada/float_array.h>
#include <collada/int_array.h>

struct dae_geometry {
        struct ref base;

        struct float_array *vertices;
        struct float_array *normals;
        struct float_array *texcoords;
        struct float_array *colors;
        struct string *id;

        /*
         * internal use
         */
        struct int_array *vertices_indexes;
};

struct dae_geometry *dae_geometry_new();
struct dae_geometry *dae_geometry_init(struct dae_geometry *p);
void dae_geometry_release(struct dae_geometry *p);

#endif
