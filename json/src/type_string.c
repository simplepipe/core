#include <stdlib.h>
#include <json/json.h>

static void __json_string_free(const struct ref *ref);

struct json_string *json_string_new()
{
	return json_string_init(malloc(sizeof(struct json_string)));
}

struct json_string *json_string_init(struct json_string *p)
{
	json_node_init(&p->base);
	p->base.base = (struct ref){__json_string_free, 1};
	p->base.type = JSON_STRING;
	p->value = string_new();
	return p;
}

void json_string_release(struct json_string *p)
{
	ref_dec(&p->value->base);
}

static void __json_string_free(const struct ref *ref)
{
	struct json_node *p = cast(ref, struct json_node, base);
	struct json_string *m = cast(p, struct json_string, base);
	
	json_string_release(m);
	json_node_release(p);
	free(m);
}
