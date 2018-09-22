#include <stdlib.h>
#include <string.h>
#include <json/json.h>

static void __check(struct json_node *p, struct string *ret);

static void __write_null(struct json_node *p, struct string *ret)
{
	string_cat_chars(ret, KEYSIZE_L("null"));
}

static void __write_string(struct json_string *p, struct string *ret)
{
	string_cat_chars(ret, KEYSIZE_L("\""));
	string_cat_chars(ret, p->value->ptr, p->value->len);
	string_cat_chars(ret, KEYSIZE_L("\""));
}

static void __write_number(struct json_number *p, struct string *ret)
{
	string_cat(ret, 
		STRING_CAT_DOUBLE_OR_INT(p->value, 6),
		NULL
	);
}

static void __write_boolean(struct json_boolean *p, struct string *ret)
{
	if(p->value) {
		string_cat_chars(ret, KEYSIZE_L("true"));
	} else {
		string_cat_chars(ret, KEYSIZE_L("false"));
	}
}

static void __write_map(struct json_map *p, struct string *ret)
{
	struct list_head *head;
	struct json_node *n;
	struct ref *ref;
	const char *key;
	unsigned i, key_len;
	string_cat_chars(ret, KEYSIZE_L("{"));
	
	head = hash_table_get_iterator(p->children);
	i = 0;
next:
	hash_table_iterator_next_value_key(p->children, &head, &ref, &key, &key_len);
	n = cast_null(ref, struct json_node, base);
	if(n) {
		if(i > 0) {
			string_cat_chars(ret, KEYSIZE_L(","));
		}
		string_cat_chars(ret, KEYSIZE_L("\""));
		string_cat_chars(ret, key, key_len);
		string_cat_chars(ret, KEYSIZE_L("\" : "));
		__check(n, ret);
		++i;
		goto next;
	}
	
	string_cat_chars(ret, KEYSIZE_L("}"));
}

static void __write_array(struct json_array *p, struct string *ret)
{
	struct json_node *n;
	unsigned i;
	
	string_cat_chars(ret, KEYSIZE_L("["));
	
	for(i = 0; i < p->children->len; ++i) {
		if(i > 0) {
			string_cat_chars(ret, KEYSIZE_L(","));
		}
		n = cast(array_get(p->children, i), struct json_node, base);
		__check(n, ret);
	}
	
	string_cat_chars(ret, KEYSIZE_L("]"));
}


static void __check(struct json_node *p, struct string *ret)
{
	switch(p->type) {
		case JSON_NODE:
			__write_null(p, ret);
			break;
		case JSON_MAP:
			__write_map(cast(p, struct json_map, base), ret);
			break;
		case JSON_ARRAY:
			__write_array(cast(p, struct json_array, base), ret);
			break;
		case JSON_STRING:
			__write_string(cast(p, struct json_string, base), ret);
			break;
		case JSON_NUMBER:
			__write_number(cast(p, struct json_number, base), ret);
			break;
		case JSON_BOOLEAN:
			__write_boolean(cast(p, struct json_boolean, base), ret);
			break;
	}
}

struct string *json_node_to_string(struct json_node *p)
{
	struct string *ret = string_new();
	
	__check(p, ret);
	
	return ret;
}
