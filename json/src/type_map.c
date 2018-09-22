#include <stdlib.h>
#include <json/json.h>

static void __json_map_free(const struct ref *ref);

struct json_map *json_map_new()
{
	return json_map_init(malloc(sizeof(struct json_map)));
}

struct json_map *json_map_init(struct json_map *p)
{
	json_node_init(&p->base);
	p->base.base = (struct ref){__json_map_free, 1};
	p->base.type = JSON_MAP;
	p->children = hash_table_new();
	return p;
}

void json_map_release(struct json_map *p)
{
	ref_dec(&p->children->base);
}

static void __json_map_free(const struct ref *ref)
{
	struct json_node *p = cast(ref, struct json_node, base);
	struct json_map *m = cast(p, struct json_map, base);
	
	json_map_release(m);
	json_node_release(p);
	free(m);
}

struct json_map *json_map_get_map(struct json_map *p, 
	const char *key, const unsigned key_len)
{
	struct json_node *n;
	struct json_map *m;
	struct ref *ref;
	
	ref = hash_table_get(p->children, key, key_len);
	n = cast_null(ref, struct json_node, base);
	if(!n || n->type != JSON_MAP) {
		m = json_map_new();
		hash_table_set(p->children, &m->base.base, key, key_len);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_map, base);
	}
	
	return m;
}

struct json_array *json_map_get_array(struct json_map *p, 
	const char *key, const unsigned key_len)
{
	struct json_node *n;
	struct json_array *m;
	struct ref *ref;
	
	ref = hash_table_get(p->children, key, key_len);
	n = cast_null(ref, struct json_node, base);
	if(!n || n->type != JSON_ARRAY) {
		m = json_array_new();
		hash_table_set(p->children, &m->base.base, key, key_len);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_array, base);
	}
	
	return m;
}

struct json_string *json_map_get_string(struct json_map *p, 
	const char *key, const unsigned key_len,
	const char *default_ptr, const unsigned default_len)
{
	struct json_node *n;
	struct json_string *m;
	struct ref *ref;
	
	ref = hash_table_get(p->children, key, key_len);
	n = cast_null(ref, struct json_node, base);
	if(!n || n->type != JSON_STRING) {
		m = json_string_new();
		string_cat_chars(m->value, default_ptr, default_len);
		hash_table_set(p->children, &m->base.base, key, key_len);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_string, base);
	}
	
	return m;
}

struct json_number *json_map_get_number(struct json_map *p, 
	const char *key, const unsigned key_len,
	const double default_v)
{
	struct json_node *n;
	struct json_number *m;
	struct ref *ref;
	
	ref = hash_table_get(p->children, key, key_len);
	n = cast_null(ref, struct json_node, base);
	if(!n || n->type != JSON_NUMBER) {
		m = json_number_new();
		m->value = default_v;
		hash_table_set(p->children, &m->base.base, key, key_len);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_number, base);
	}
	
	return m;	
}

struct json_boolean *json_map_get_boolean(struct json_map *p, 
	const char *key, const unsigned key_len,
	const unsigned default_v)
{
	struct json_node *n;
	struct json_boolean *m;
	struct ref *ref;
	
	ref = hash_table_get(p->children, key, key_len);
	n = cast_null(ref, struct json_node, base);
	if(!n || n->type != JSON_BOOLEAN) {
		m = json_boolean_new();
		m->value = default_v;
		hash_table_set(p->children, &m->base.base, key, key_len);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_boolean, base);
	}
	
	return m;
}
