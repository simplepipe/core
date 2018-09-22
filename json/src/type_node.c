#include <stdlib.h>
#include <json/json.h>

static void __json_free(const struct ref *ref);

struct json_node *json_node_new()
{
	return json_node_init(malloc(sizeof(struct json_node)));
}

struct json_node *json_node_init(struct json_node *p)
{
	p->base = (struct ref){__json_free, 1};
	p->type = JSON_NODE;
	return p;
}

void json_node_release(struct json_node *p)
{
	
}

static void __json_free(const struct ref *ref)
{
	struct json_node *p = cast(ref, struct json_node, base);
	json_node_release(p);
	free(p);
}


