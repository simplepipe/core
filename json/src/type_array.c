#include <stdlib.h>
#include <json/json.h>

static void __json_array_free(const struct ref *ref);

struct json_array *json_array_new()
{
	return json_array_init(malloc(sizeof(struct json_array)));
}

struct json_array *json_array_init(struct json_array *p)
{
	json_node_init(&p->base);
	p->base.base = (struct ref){__json_array_free, 1};
	p->base.type = JSON_ARRAY;
	p->children = array_new(1);
	return p;
}

void json_array_release(struct json_array *p)
{
	ref_dec(&p->children->base);
}

static void __json_array_free(const struct ref *ref)
{
	struct json_node *p = cast(ref, struct json_node, base);
	struct json_array *m = cast(p, struct json_array, base);
	
	json_array_release(m);
	json_node_release(p);
	free(m);
}

struct json_map *json_array_get_map(struct json_array *p, unsigned id)
{
	struct ref *ref;
	struct json_node *n;
	struct json_map *m;
	
	ref = array_get(p->children, id);
	n = cast_null(ref, struct json_node, base);
	if(!n) {
		return NULL;
	} 
	
	if(n->type != JSON_MAP) {
		m = json_map_new();
		array_set(p->children, &m->base.base, id);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_map, base);
	}
	
	return m;
}

struct json_array *json_array_get_array(struct json_array *p, unsigned id)
{
	struct ref *ref;
	struct json_node *n;
	struct json_array *m;
	
	ref = array_get(p->children, id);
	n = cast_null(ref, struct json_node, base);
	if(!n) {
		return NULL;
	} 
	
	if(n->type != JSON_ARRAY) {
		m = json_array_new();
		array_set(p->children, &m->base.base, id);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_array, base);
	}
	
	return m;
}

struct json_string *json_array_get_string(struct json_array *p, unsigned id,
	const char *default_ptr, const unsigned default_len)
{
	struct ref *ref;
	struct json_node *n;
	struct json_string *m;
	
	ref = array_get(p->children, id);
	n = cast_null(ref, struct json_node, base);
	if(!n) {
		return NULL;
	} 
	
	if(n->type != JSON_STRING) {
		m = json_string_new();
		string_cat_chars(m->value, default_ptr, default_len);
		array_set(p->children, &m->base.base, id);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_string, base);
	}
	
	return m;
}

struct json_number *json_array_get_number(struct json_array *p, unsigned id,
	const double default_v)
{
	struct ref *ref;
	struct json_node *n;
	struct json_number *m;
	
	ref = array_get(p->children, id);
	n = cast_null(ref, struct json_node, base);
	if(!n) {
		return NULL;
	} 
	
	if(n->type != JSON_NUMBER) {
		m = json_number_new();
		m->value = default_v;
		array_set(p->children, &m->base.base, id);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_number, base);
	}
	
	return m;
}

struct json_boolean *json_array_get_boolean(struct json_array *p, unsigned id,
	const unsigned default_v)
{
	struct ref *ref;
	struct json_node *n;
	struct json_boolean *m;
	
	ref = array_get(p->children, id);
	n = cast_null(ref, struct json_node, base);
	if(!n) {
		return NULL;
	} 
	
	if(n->type != JSON_BOOLEAN) {
		m = json_boolean_new();
		m->value = default_v;
		array_set(p->children, &m->base.base, id);
		ref_dec(&m->base.base);
	} else {
		m = cast(n, struct json_boolean, base);
	}
	
	return m;
}
