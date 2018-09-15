#ifndef __collada_context_h
#define __collada_context_h

#include <kernel/hash_table.h>
#include <collada/geometry.h>
#include <collada/skin.h>
#include <collada/amature.h>
#include <collada/node.h>

struct collada_context {
        struct ref base;

        struct hash_table *geometries;
        struct hash_table *skines;
        struct hash_table *amatures;
        struct hash_table *nodes;
};

struct collada_context *collada_context_new();
struct collada_context *collada_context_init(struct collada_context *p);
void collada_context_release(struct collada_context *p);

void collada_context_parse(struct collada_context *p, const char *file);

#endif
