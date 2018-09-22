#include <stdio.h>
#include <json/json.h>

int main(int argc, char **argv)
{
	struct json_node *n = json_node_from_file("inner://res/test.json");
	
	if(n) {
		if(n->type == JSON_MAP) {
			json_map_get_string(
				cast(n, struct json_map, base),
				KEYSIZE_L("test"),
				KEYSIZE_L("test_content")
			);
			
			json_map_get_boolean(
				cast(n, struct json_map, base),
				KEYSIZE_L("test"),
				1
			);
		}
		struct string *s = json_node_to_string(n);
		printf("%s\n", s->ptr);
	
		ref_dec(&n->base);
		ref_dec(&s->base);
		
	}

	return 0;
}
