#ifndef __json_json_h
#define __json_json_h

#include <kernel/string.h>
#include <kernel/array.h>
#include <kernel/hash_table.h>

enum {
	JSON_NODE,
        JSON_MAP,
        JSON_ARRAY,
        JSON_STRING,
        JSON_NUMBER,
        JSON_BOOLEAN
};

struct json_node {
        struct ref base;
       	unsigned type;
};

struct json_map {
        struct json_node base;

        struct hash_table *children;
};

struct json_array {
        struct json_node base;

        struct array *children;
};

struct json_string {
	struct json_node base;
	struct string *value;
};

struct json_number {
	struct json_node base;
	double value;
};

struct json_boolean {
	struct json_node base;
	
	unsigned value : 1;
};

struct string *json_node_to_string(struct json_node *p);
struct json_node *json_node_from_file(char *file);
struct json_node *json_node_from_buffer(const char *buf, 
	const unsigned len);
struct json_node *json_node_new();
struct json_node *json_node_init(struct json_node *p);
void json_node_release(struct json_node *p);

struct json_map *json_map_new();
struct json_map *json_map_init(struct json_map *p);
void json_map_release(struct json_map *p);
struct json_map *json_map_get_map(struct json_map *p, 
	const char *key, const unsigned key_len);
struct json_array *json_map_get_array(struct json_map *p, 
	const char *key, const unsigned key_len);
struct json_string *json_map_get_string(struct json_map *p, 
	const char *key, const unsigned key_len,
	const char *default_ptr, const unsigned default_len);
struct json_number *json_map_get_number(struct json_map *p, 
	const char *key, const unsigned key_len,
	const double default_v);
struct json_boolean *json_map_get_boolean(struct json_map *p, 
	const char *key, const unsigned key_len,
	const unsigned default_v);

struct json_array *json_array_new();
struct json_array *json_array_init(struct json_array *p);
void json_array_release(struct json_array *p);
struct json_map *json_array_get_map(struct json_array *p, unsigned id);
struct json_array *json_array_get_array(struct json_array *p, unsigned id);
struct json_string *json_array_get_string(struct json_array *p, unsigned id,
	const char *default_ptr, const unsigned default_len);
struct json_number *json_array_get_number(struct json_array *p, unsigned id,
	const double default_v);
struct json_boolean *json_array_get_boolean(struct json_array *p, unsigned id,
	const unsigned default_v);

struct json_string *json_string_new();
struct json_string *json_string_init(struct json_string *p);
void json_string_release(struct json_string *p);

struct json_number *json_number_new();
struct json_number *json_number_init(struct json_number *p);
void json_number_release(struct json_number *p);

struct json_boolean *json_boolean_new();
struct json_boolean *json_boolean_init(struct json_boolean *p);
void json_boolean_release(struct json_boolean *p);

#endif
