#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <kernel/utf8.h>
#include <json/json.h>
#include <kernel/io.h>

enum {
	STATE_ERROR,
	STATE_OBJECT,
	STATE_ARRAY,
	STATE_VALUE
};

struct pstring {
	const char * ptr;
	unsigned len;
};

struct param {
	struct array *chains;
	unsigned state;
	u8 width;
	const char *ptr;
	struct pstring pstring[2];
};

static void __pop(struct param *param);
static struct json_node *__last(struct param *param);
static void __push_map(struct param *param);
static void __push_array(struct param *param);
static void __push_string(struct param *param);
static void __push_number(struct param *param);
static void __push_boolean(struct param *param, unsigned value);
static void __push_null(struct param *param);

static void __pop(struct param *param)
{
	struct json_node *l;
	
	array_remove(param->chains, param->chains->len - 1);
	l = __last(param);
	if(l) {
		switch(l->type) {
			case JSON_MAP:
				param->state = STATE_OBJECT;
				break;
			case JSON_ARRAY:
				param->state = STATE_ARRAY;
				break;
			default:
				param->state = STATE_ERROR;
				break;
		}
	} else {
		param->state = STATE_ERROR;
	}
}

static struct json_node *__last(struct param *param)
{
	return cast_null(array_get(param->chains, param->chains->len - 1), 
		struct json_node, base);
}

static void __push_map(struct param *param)
{
	struct json_node *l;
	struct json_map *m, *lm;
	struct json_array *la;
	
	m = json_map_new();
	
	l = __last(param);
	if(!l) goto finish;
	
	switch(l->type) {
		case JSON_MAP:
			lm = cast(l, struct json_map, base);
			hash_table_set(lm->children, &m->base.base, 
				param->pstring[0].ptr, param->pstring[0].len);
			break;
		case JSON_ARRAY:
			la = cast(l, struct json_array, base);
			array_push(la->children, &m->base.base);
			break;
	}
finish:
	array_push(param->chains, &m->base.base);
	ref_dec(&m->base.base);
	param->state = STATE_OBJECT;
}

static void __push_array(struct param *param)
{
	struct json_node *l;
	struct json_map *lm;
	struct json_array *a, *la;
	
	a = json_array_new();
	
	l = __last(param);
	if(!l) goto finish;
	
	switch(l->type) {
		case JSON_MAP:
			lm = cast(l, struct json_map, base);
			hash_table_set(lm->children, &a->base.base,
				param->pstring[0].ptr, param->pstring[0].len);
			break;
		case JSON_ARRAY:
			la = cast(l, struct json_array, base);
			array_push(la->children, &a->base.base);
			break;
	}
finish:
	array_push(param->chains, &a->base.base);
	ref_dec(&a->base.base);
	param->state = STATE_ARRAY;
}

static void __push_string(struct param *param)
{
	struct json_node *l;
	struct json_map *lm;
	struct json_array *la;
	struct json_string *s;
	
	s = json_string_new();
	string_cat_chars(s->value, param->pstring[1].ptr, param->pstring[1].len);
	
	l = __last(param);
	if(!l) goto finish;
	
	switch(l->type) {
		case JSON_MAP:
			param->state = STATE_OBJECT;
			lm = cast(l, struct json_map, base);
			hash_table_set(lm->children, &s->base.base, 
				param->pstring[0].ptr, param->pstring[0].len);
			break;
		case JSON_ARRAY:
			param->state = STATE_ARRAY;
			la = cast(l, struct json_array, base);
			array_push(la->children, &s->base.base);
			break;
	}
finish:
	ref_dec(&s->base.base);
}

static void __push_number(struct param *param)
{
	struct json_node *l;
	struct json_map *lm;
	struct json_array *la;
	struct json_number *n;
	char buf[64];
	
	n = json_number_new();
	memcpy(buf, param->pstring[1].ptr, param->pstring[1].len);
	buf[param->pstring[1].len] = '\0';
	setlocale(LC_NUMERIC, "C");
	n->value = atof(buf);
	
	l = __last(param);
	if(!l) goto finish;
	
	switch(l->type) {
		case JSON_MAP:
			param->state = STATE_OBJECT;
			lm = cast(l, struct json_map, base);
			hash_table_set(lm->children, &n->base.base,
				param->pstring[0].ptr, param->pstring[0].len);
			break;
		case JSON_ARRAY:
			param->state = STATE_ARRAY;
			la = cast(l, struct json_array, base);
			array_push(la->children, &n->base.base);
			break;
	}
finish:
	ref_dec(&n->base.base);
}

static void __push_boolean(struct param *param, unsigned value)
{
	struct json_node *l;
	struct json_map *lm;
	struct json_array *la;
	struct json_boolean *b;
	
	b = json_boolean_new();
	b->value = value;
	
	l = __last(param);
	if(!l) goto finish;
	
	switch(l->type) {
		case JSON_MAP:
			param->state = STATE_OBJECT;
			lm = cast(l, struct json_map, base);
			hash_table_set(lm->children, &b->base.base, 
				param->pstring[0].ptr, param->pstring[0].len);
			break;
		case JSON_ARRAY:
			param->state = STATE_ARRAY;
			la = cast(l, struct json_array, base);
			array_push(la->children, &b->base.base);
			break;
	}
finish:
	ref_dec(&b->base.base);
}

static void __push_null(struct param *param)
{
	struct json_node *l;
	struct json_map *lm;
	struct json_array *la;
	struct json_node *b;
	
	b = json_node_new();
	
	l = __last(param);
	if(!l) goto finish;
	
	switch(l->type) {
		case JSON_MAP:
			param->state = STATE_OBJECT;
			lm = cast(l, struct json_map, base);
			hash_table_set(lm->children, &b->base, 
				param->pstring[0].ptr, param->pstring[0].len);
			break;
		case JSON_ARRAY:
			param->state = STATE_ARRAY;
			la = cast(l, struct json_array, base);
			array_push(la->children, &b->base);
			break;
	}
finish:
	ref_dec(&b->base);
}

static void __param_next(struct param *p)
{
	p->ptr += p->width;
	p->width = utf8_width(p->ptr);
}

static void __read_string(struct param *param, unsigned index)
{
	param->pstring[index].ptr = param->ptr + 1;
	param->pstring[index].len = 0;
	
next:	
	__param_next(param);
	
	switch(*param->ptr) {
		case '\\':
			__param_next(param);
			goto next;
		case '\"':
			param->pstring[index].len = 
				param->ptr - param->pstring[index].ptr;
			break;
		case '\0':
			param->state = STATE_ERROR;
			break;
		default:
			goto next;
	}
}

static void __read_number(struct param *param)
{
	param->pstring[1].ptr = param->ptr;
	
	if(*param->ptr == '-') {
		__param_next(param);
	}

decimal:
	switch(*param->ptr) {
		case '0':case '1':case '2':case '3':
		case '4':case '5':case '6':case '7':case '8':case '9':
			__param_next(param);
			goto decimal;
		case '.':
			__param_next(param);
			break;
		case 'e': case 'E':
			__param_next(param);
			goto pre_expo;
		default:
			goto finish;
	}
	
fraction:
	switch(*param->ptr) {
		case '0':case '1':case '2':case '3':
		case '4':case '5':case '6':case '7':case '8':case '9':
			__param_next(param);
			goto fraction;
		case 'e': case 'E':
			__param_next(param);
			break;
		default:
			goto finish;
	}

pre_expo:	
	switch(*param->ptr) {
		case '+': case '-':
			__param_next(param);
			break;
	}
expo:
	switch(*param->ptr) {
		case '0':case '1':case '2':case '3':
		case '4':case '5':case '6':case '7':case '8':case '9':
			__param_next(param);
			goto expo;
		default:
			goto finish;
	}
	
	
finish:
	param->pstring[1].len = param->ptr - param->pstring[1].ptr;
}

static void __state_value(struct param *param)
{
	switch(*param->ptr) {
		case ' ': case '\t': case '\n':
		case '\v': case '\f': case '\r':
			break;
		case '\"':
			__read_string(param, 1);
			if(param->state != STATE_ERROR) {
				__push_string(param);
			}
			break;
		case '-':case '0':case '1':case '2':case '3':
		case '4':case '5':case '6':case '7':case '8':case '9':
			__read_number(param);
			if(param->state != STATE_ERROR) {
				__push_number(param);
			}
			break;
		case '{':
			__push_map(param);
			break;
		case '[':
			__push_array(param);
			break;
		case 't':
			if(strncmp(param->ptr, "true", 4) == 0) {
				__push_boolean(param, 1);
				__param_next(param);
				__param_next(param);
				__param_next(param);
			} else {
				param->state = STATE_ERROR;
			}
			break;
		case 'f':
			if(strncmp(param->ptr, "false", 5) == 0) {
				__push_boolean(param, 0);
				__param_next(param);
				__param_next(param);
				__param_next(param);
				__param_next(param);
			} else {
				param->state = STATE_ERROR;
			}
			break;
		case 'n':
			if(strncmp(param->ptr, "null", 4) == 0) {
				__push_null(param);
				__param_next(param);
				__param_next(param);
				__param_next(param);
			} else {
				param->state = STATE_ERROR;
			}
			break;
		case '\0':
			param->state = STATE_ERROR;
			break;
		default:
			param->state = STATE_ERROR;
			break;
	}
}

static void __state_object(struct param *param)
{
	switch(*param->ptr) {
		case ',':
			break;
		case ' ': case '\t': case '\n':
		case '\v': case '\f': case '\r':
			break;
		case '}':
			__pop(param);
			break;
		case '\"':
			__read_string(param, 0);
			break;
		case ':':
			param->state = STATE_VALUE;
			break;
		default:
			param->state = STATE_ERROR;
			break;
	}
}

static void __state_array(struct param *param)
{
	switch(*param->ptr) {
		case ',':
			break;
		case ' ': case '\t': case '\n':
		case '\v': case '\f': case '\r':
			break;
		case ']':
			__pop(param);
			break;
		default:
			param->state = STATE_VALUE;
			__state_value(param);
			break;
	}
}

struct json_node *json_node_from_buffer(const char *buf, 
	const unsigned len)
{
	struct param param;
	struct json_node *n;
	struct json_array *a;
	
	param.chains = array_new(1);
	__push_array(&param);
	
	param.ptr = buf;
	param.width = utf8_width(param.ptr);
	param.state = STATE_VALUE;
	param.pstring[0].ptr = NULL;
	param.pstring[0].len = 0;
	param.pstring[1].ptr = NULL;
	param.pstring[1].len = 0;
	
	while(*param.ptr) {
		switch(param.state) {
			case STATE_VALUE:
				__state_value(&param);
				break;
			case STATE_OBJECT:
				__state_object(&param);
				break;
			case STATE_ARRAY:
				__state_array(&param);
				break;
		}
		if(param.state == STATE_ERROR) {
			array_clear(param.chains);
			break;
		}
		__param_next(&param);		
	}

finish:
	n = cast_null(array_get(param.chains, 0), struct json_node, base);
	if(n) {
		if(n->type == JSON_ARRAY) {
			a = cast(n, struct json_array, base);
			n = cast_null(array_get(a->children, 0), struct json_node, base);
			ref_inc(&n->base);
		} else {
			n = NULL;
		}
	}
	
	ref_dec(&param.chains->base);
	return n;
}

struct json_node *json_node_from_file(char *file)
{
	struct string *buf = file_to_string(file);
	struct json_node *n = json_node_from_buffer(buf->ptr, buf->len);
	ref_dec(&buf->base);
	return n;
}
