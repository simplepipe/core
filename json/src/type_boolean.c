#include <stdlib.h>
#include <json/json.h>

static void __json_boolean_free(const struct ref *ref);

struct json_boolean *json_boolean_new()
{
	return json_boolean_init(malloc(sizeof(struct json_boolean)));
}

struct json_boolean *json_boolean_init(struct json_boolean *p)
{
	json_node_init(&p->base);
	p->base.base = (struct ref){__json_boolean_free, 1};
	p->base.type = JSON_BOOLEAN;
	p->value = 0;
	return p;
}

void json_boolean_release(struct json_boolean *p)
{
}

static void __json_boolean_free(const struct ref *ref)
{
	struct json_node *p = cast(ref, struct json_node, base);
	struct json_boolean *m = cast(p, struct json_boolean, base);
	
	json_boolean_release(m);
	json_node_release(p);
	free(m);
}
