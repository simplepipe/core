#ifndef __xml_xml_h
#define __xml_xml_h

#include <setjmp.h>
#include <kernel/ref.h>
#include <kernel/string.h>
#include <kernel/hash_table.h>
#include <kernel/array.h>

struct xml_attribute {
        struct ref base;

        const char *name;
        const unsigned name_len;

        const char *value;
        const unsigned value_len;
};

struct xml_node {
        struct ref base;

        const char *name;
        const unsigned name_len;

        const char *value;
        const unsigned value_len;

        struct hash_table *attributes;
        struct array *nodes;
        struct xml_node *parent;
};

struct xml_context {
        struct ref base;

        struct xml_node *root;

        const struct string *data;
};

struct xml_context *xml_context_new();
struct xml_context *xml_context_init(struct xml_context *p);
void xml_context_release(struct xml_context *p);
void xml_context_parse(struct xml_context *p, const char *path);

struct xml_node *xml_node_new_mask(
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);
struct xml_node *xml_node_new(
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);
void xml_node_release(struct xml_node *p);
void xml_node_add_attribute(struct xml_node *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);
void xml_node_add_attribute_mask(struct xml_node *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);
void xml_node_add_node(struct xml_node *p, struct xml_node *c);
struct xml_attribute *xml_node_get_attribute(struct xml_node *p,
        const char *key, const unsigned key_len);
void xml_node_search(struct xml_node *p, const char *command,
        struct array *result);
int xml_node_has_attribute(struct xml_node *p);

struct xml_attribute *xml_attribute_new_mask(
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);
struct xml_attribute *xml_attribute_new(
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);
void xml_attribute_release(struct xml_attribute *p);

#endif
