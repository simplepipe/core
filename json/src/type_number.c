#include <stdlib.h>
#include <json/json.h>

static void __json_number_free(const struct ref *ref);

struct json_number *json_number_new()
{
	return json_number_init(malloc(sizeof(struct json_number)));
}

struct json_number *json_number_init(struct json_number *p)
{
	json_node_init(&p->base);
	p->base.base = (struct ref){__json_number_free, 1};
	p->base.type = JSON_NUMBER;
	p->value = 0.0;
	return p;
}

void json_number_release(struct json_number *p)
{
}

static void __json_number_free(const struct ref *ref)
{
	struct json_node *p = cast(ref, struct json_node, base);
	struct json_number *m = cast(p, struct json_number, base);
	
	json_number_release(m);
	json_node_release(p);
	free(m);
}
