#ifndef __collada_context_h
#define __collada_context_h

#include <kernel/hash_table.h>
#include <collada/geometry.h>
#include <collada/skin.h>
#include <collada/amature.h>
#include <collada/node.h>

struct dae_context {
        struct ref base;

        struct hash_table *geometries;
        struct hash_table *skines;
        struct hash_table *amatures;
        struct hash_table *nodes;
};

struct dae_context *dae_context_new();
struct dae_context *dae_context_init(struct dae_context *p);
void dae_context_release(struct dae_context *p);

void dae_context_parse(struct dae_context *p, const char *file);

#endif
