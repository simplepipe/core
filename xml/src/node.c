#include <stdlib.h>
#include <string.h>
#include <kernel/utf8.h>
#include <xml/xml.h>

static const char *null = (char[2]){'\0', '\0'};

static inline void __set_reverse(const unsigned n,
        const char **restrict l, const char * restrict r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] = &r[n - i - 1];
        }
}

static inline void __xml_node_check_attributes(struct xml_node *p);
static inline void __xml_node_check_nodes(struct xml_node *p);
static void __xml_node_free(const struct ref *ref);
static struct xml_node *xml_node_init(struct xml_node *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);
static struct xml_node *xml_node_init_mask(struct xml_node *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);

struct xml_node *xml_node_new(const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        char *ptr;

        struct xml_node *p = xml_node_init(malloc(sizeof(struct xml_node)
                + name_len + value_len + 2), name, name_len, value, value_len);

        ptr = (char *)p;
        ptr += sizeof(struct xml_node);

        if(name_len) {
                memcpy(ptr, name, name_len);
                ptr += name_len;
        }
        ptr++;

        if(value_len) {
                memcpy(ptr, value, value_len);
        }

        return p;
}

static struct xml_node *xml_node_init(struct xml_node *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        char *ptr;

        p->base = (struct ref){__xml_node_free, 1};
        p->attributes = NULL;
        p->nodes = NULL;
        p->parent = NULL;

        ptr = (char *)p;
        ptr += sizeof(struct xml_node);

        p->name = ptr;
        ptr += name_len;
        *ptr = '\0';
        ptr++;

        p->value = ptr;
        ptr += value_len;
        *ptr = '\0';

        *(unsigned *)&p->name_len = name_len;
        *(unsigned *)&p->value_len = value_len;

        return p;
}

struct xml_node *xml_node_new_mask(const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        return xml_node_init_mask(malloc(sizeof(struct xml_node)),
                name, name_len, value, value_len);
}

static struct xml_node *xml_node_init_mask(struct xml_node *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        p->base = (struct ref){__xml_node_free, 1};
        p->attributes = NULL;
        p->nodes = NULL;
        p->parent = NULL;

        p->name = name;
        p->value = value;

        *(unsigned *)&p->name_len = name_len;
        *(unsigned *)&p->value_len = value_len;

        return p;
}

void xml_node_release(struct xml_node *p)
{
        if(p->attributes) {
                ref_dec(&p->attributes->base);
                p->attributes = NULL;
        }
        if(p->nodes) {
                ref_dec(&p->nodes->base);
                p->nodes = NULL;
        }
}

static void __xml_node_free(const struct ref *ref)
{
        struct xml_node *p = cast(ref, struct xml_node, base);
        xml_node_release(p);
        free(p);
}

static inline void __xml_node_check_attributes(struct xml_node *p)
{
        if(!p->attributes) {
                p->attributes = hash_table_new();
        }
}

static inline void __xml_node_check_nodes(struct xml_node *p)
{
        if(!p->nodes) {
                p->nodes = array_new(1);
        }
}

void xml_node_add_attribute(struct xml_node *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        struct xml_attribute *a;

        __xml_node_check_attributes(p);

        a = xml_attribute_new(name, name_len, value, value_len);
        hash_table_set(p->attributes, &a->base, name, name_len);
        ref_dec(&a->base);
}

void xml_node_add_attribute_mask(struct xml_node *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        struct xml_attribute *a;

        __xml_node_check_attributes(p);

        a = xml_attribute_new_mask(name, name_len, value, value_len);
        hash_table_set(p->attributes, &a->base, name, name_len);
        ref_dec(&a->base);
}

void xml_node_add_node(struct xml_node *p, struct xml_node *c)
{
        __xml_node_check_nodes(p);
        array_push(p->nodes, &c->base);
        c->parent = p;
}

struct xml_attribute *xml_node_get_attribute(struct xml_node *p,
        const char *key, const unsigned key_len)
{
        struct ref *ref = NULL;

        if(!p->attributes) goto finish;

        ref = hash_table_get(p->attributes, key, key_len);

finish:
        return cast_null(ref, struct xml_attribute, base);
}

int xml_node_has_attribute(struct xml_node *p)
{
        if(!p->attributes) return 0;

        return list_singular(&p->attributes->iterators) ? 0 : 1;
}

static void __block_attribute(const char *start, const char *end,
        const char **name_begin, const char **name_end,
        const char **value_begin, const char **value_end)
{
        const char *ptr, *value[2], *name[2];
        char value_mark;
        u8 width;

        *name_begin = &null[1];
        *name_end = &null[0];
        *value_begin = &null[1];
        *value_end = &null[0];

        __set_reverse(2, name, null);
        __set_reverse(2, value, null);
        ptr = start;
find_name:
        while(ptr <= end) {
                switch (*ptr) {
                        case '@':
                                name[0] = ptr + 1;
                                ptr++;
                                break;
                        case ' ': case '\t': case '\n':
                        case '\v': case '\f': case '\r':
                        case '=':
                                if(*name[0]) {
                                        name[1] = ptr - 1;
                                        ptr++;
                                        if(name[1] < name[0]) {
                                                goto finish;
                                        }
                                        goto find_value;
                                } else {
                                        ptr++;
                                        break;
                                }
                        default:
                                width = utf8_width(ptr);
                                ptr += width;
                                break;
                }
        }
        goto finish;

find_value:
        value_mark = 0;
        while(ptr <= end) {
                switch (*ptr) {
                        case '\'': case '\"':
                                if(*ptr == value_mark) {
                                        value[1] = ptr - 1;
                                        goto assign;
                                } else {
                                        value_mark = *ptr;
                                        value[0] = ptr + 1;
                                }
                                ptr++;
                                break;
                        default:
                                width = utf8_width(ptr);
                                ptr += width;
                                break;
                }
        }
        goto finish;

assign:
        *name_begin = name[0];
        *name_end = name[1];
        *value_begin = value[0];
        *value_end = value[1];

finish:
        ;
}

static void __block_name(const char *start, const char *end, const char **mid)
{
        const char *ptr;
        u8 width;

        ptr = start;
        *mid = &null[0];

        while(*ptr && ptr <= end) {
                switch (*ptr) {
                        case '[':
                                *mid = ptr - 1;
                                goto finish;
                        default:
                                *mid = ptr;
                                width = utf8_width(ptr);
                                ptr += width;
                                break;
                }
        }
finish:
        ;
}

static void __block(const char *str, const char **start, const char **end)
{
        const char *ptr;
        u8 width;

        ptr = str;
head:
        switch (*ptr) {
                case ' ': case '\t': case '\n':
                case '\v': case '\f': case '\r':
                        ptr++;
                        goto head;
                case '/':
                        *start = ptr + 1;
                        if(**start == '/') {
                                *end = ptr;
                                goto finish;
                        }
                        ptr++;
                        break;
                default:
                        *start = &null[1];
                        *end = &null[0];
                        goto finish;
        }
tail:
        switch (*ptr) {
                case '/': case '\0':
                        *end = ptr - 1;
                        goto finish;
                default:
                        width = utf8_width(ptr);
                        ptr += width;
                        goto tail;
        }
finish:
        ;
}

static void __fetch_attribute_name(struct array *result,
        const char *name, const unsigned len)
{
        unsigned i;
        struct array *attributes;
        struct hash_table *set;
        struct list_head *it;
        struct xml_node *n;
        struct xml_attribute *a;
        struct ref *ref;

        attributes = array_new(1);
        set = hash_table_new();

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                if(!hash_table_get(set, &n, sizeof(n))) {
                        hash_table_set(set, &n->base, &n, sizeof(n));

                        if(!n->attributes) continue;

                        it = hash_table_get_iterator(n->attributes);
                        while((ref = hash_table_iterator_next(
                                n->attributes, &it))) {
                                a = cast(ref, struct xml_attribute, base);
                                if(a->name_len == len
                                        && memcmp(a->name, name, len) == 0) {
                                        array_push(attributes, ref);
                                }
                        }
                }
        }

        array_swap(result, attributes);

        ref_dec(&attributes->base);
        ref_dec(&set->base);
}

static void __fetch_all_attribute(struct array *result)
{
        unsigned i;
        struct array *attributes;
        struct hash_table *set;
        struct list_head *it;
        struct xml_node *n;
        struct ref *ref;

        attributes = array_new(1);
        set = hash_table_new();

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                if(!hash_table_get(set, &n, sizeof(n))) {
                        hash_table_set(set, &n->base, &n, sizeof(n));

                        if(!n->attributes) continue;

                        it = hash_table_get_iterator(n->attributes);
                        while((ref = hash_table_iterator_next(
                                n->attributes, &it))) {
                                array_push(attributes, ref);
                        }
                }
        }

        array_swap(result, attributes);

        ref_dec(&attributes->base);
        ref_dec(&set->base);
}

static void __keep_node_has_at_least_one_attribute(struct array *result)
{
        unsigned i;
        struct xml_node *n;
        struct xml_attribute *a;

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                if(!xml_node_has_attribute(n)) {
                        array_remove(result, i);
                        i--;
                }
        }
}

static void __keep_node_has_at_least_one_attribute_with_value(
        struct array *result,
        const char *value, const unsigned value_len)
{
        unsigned i;
        struct xml_node *n;
        struct xml_attribute *a;
        struct ref *ref;
        struct list_head *it;

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);

                if(!n->attributes) continue;

                it = hash_table_get_iterator(n->attributes);
                while((ref = hash_table_iterator_next(
                        n->attributes, &it))) {
                        a = cast(ref, struct xml_attribute, base);
                        if(a->value_len == value_len
                                && memcmp(a->value, value, value_len) == 0) {
                                goto end;
                        }
                }

                array_remove(result, i);
                i--;

        end:
                ;
        }
}

static void __keep_node_has_attribute(struct array *result,
        const char *name, const unsigned name_len)
{
        unsigned i;
        struct xml_node *n;
        struct xml_attribute *a;

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                a = xml_node_get_attribute(n, name, name_len);
                if(!a) {
                        array_remove(result, i);
                        i--;
                }
        }
}

static void __keep_node_has_attribute_with_value(struct array *result,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        unsigned i;
        struct xml_node *n;
        struct xml_attribute *a;

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                a = xml_node_get_attribute(n, name, name_len);
                if(!a) {
                        array_remove(result, i);
                        i--;
                } else {
                        if(a->value_len != value_len
                                || memcmp(a->value, value, value_len) != 0) {
                                array_remove(result, i);
                                i--;
                        }
                }
        }
}

static void __move_to_parent(struct array *result)
{
        unsigned i;
        struct xml_node *n;
        struct hash_table *set;

        set = hash_table_new();

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                if(n->parent) {
                        if(!hash_table_get(set, &n->parent, sizeof(n->parent))) {
                                array_set(result, &n->parent->base, i);
                                hash_table_set(set, &n->parent->base,
                                         &n->parent, sizeof(n->parent));
                        } else {
                                array_remove(result, i);
                                i--;
                        }
                } else {
                        array_remove(result, i);
                        i--;
                }
        }

        ref_dec(&set->base);
}

static void __get_all_children_has_name(struct array *result,
        const char *name, const unsigned len)
{
        struct array *children;
        unsigned i, j;
        struct xml_node *n, *c;
        struct hash_table *set;

        set = hash_table_new();
        children = array_new(1);

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                if(!n->nodes) continue;

                for(j = 0; j < n->nodes->len; ++j) {
                        c = cast(array_get(n->nodes, j), struct xml_node, base);
                        if(!hash_table_get(set, &c, sizeof(c))) {
                                hash_table_set(set, &c->base, &c, sizeof(c));
                                if(c->name_len == len
                                        && memcmp(c->name, name, len) == 0) {
                                        array_push(children, &c->base);
                                }
                        }
                }
        }

        array_swap(result, children);

        ref_dec(&children->base);
        ref_dec(&set->base);
}

static void __get_all_children(struct array *result)
{
        struct array *children;
        unsigned i, j;
        struct xml_node *n, *c;
        struct hash_table *set;

        set = hash_table_new();
        children = array_new(1);

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                if(!n->nodes) continue;

                for(j = 0; j < n->nodes->len; ++j) {
                        c = cast(array_get(n->nodes, j), struct xml_node, base);
                        if(!hash_table_get(set, &c, sizeof(c))) {
                                array_push(children, &c->base);
                                hash_table_set(set, &c->base, &c, sizeof(c));
                        }
                }
        }

        array_swap(result, children);

        ref_dec(&children->base);
        ref_dec(&set->base);
}

static void __get_all_children_has_name_recursive(struct array *result,
        const char *name, const unsigned len)
{
        struct array *children;
        unsigned i, j;
        struct xml_node *n, *c;
        struct hash_table *set, *check;

        set = hash_table_new();
        check = hash_table_new();
        children = array_new(1);

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                hash_table_set(check, &n->base, &n, sizeof(n));
        }

        result->ordered = 1;
        i = 0;
        while(i < result->len) {
                n = cast(array_get(result, i), struct xml_node, base);
                if(!n->nodes) goto end;

                for(j = 0; j < n->nodes->len; ++j) {
                        c = cast(array_get(n->nodes, j), struct xml_node, base);
                        if(!hash_table_get(set, &c, sizeof(c))) {
                                hash_table_set(set, &c->base, &c, sizeof(c));
                                if(c->name_len == len
                                        && memcmp(c->name, name, len) == 0) {
                                        array_push(children, &c->base);
                                }
                        }
                        if(!hash_table_get(check, &c, sizeof(c))) {
                                hash_table_set(check, &c->base, &c, sizeof(c));
                                array_push(result, &c->base);
                        }
                }

        end:
                i++;
        }

        array_swap(result, children);

        ref_dec(&children->base);
        ref_dec(&set->base);
        ref_dec(&check->base);
}

static void __get_all_children_recursive(struct array *result)
{
        struct array *children;
        unsigned i, j;
        struct xml_node *n, *c;
        struct hash_table *set, *check;

        set = hash_table_new();
        check = hash_table_new();
        children = array_new(1);

        for(i = 0; i < result->len; ++i) {
                n = cast(array_get(result, i), struct xml_node, base);
                hash_table_set(check, &n->base, &n, sizeof(n));
        }

        result->ordered = 1;
        i = 0;
        while(i < result->len) {
                n = cast(array_get(result, i), struct xml_node, base);
                if(!n->nodes) goto end;

                for(j = 0; j < n->nodes->len; ++j) {
                        c = cast(array_get(n->nodes, j), struct xml_node, base);
                        if(!hash_table_get(set, &c, sizeof(c))) {
                                array_push(children, &c->base);
                                hash_table_set(set, &c->base, &c, sizeof(c));
                        }
                        if(!hash_table_get(check, &c, sizeof(c))) {
                                hash_table_set(check, &c->base, &c, sizeof(c));
                                array_push(result, &c->base);
                        }
                }

        end:
                i++;
        }

        array_swap(result, children);

        ref_dec(&children->base);
        ref_dec(&set->base);
        ref_dec(&check->base);
}

void xml_node_search(struct xml_node *p, const char *command,
        struct array *result)
{
        const char *start, *end, *mid, *ptr;
        const char *name[2], *value[2];
        int sub;
        u8 recursive;
        struct ref *ref;

        array_clear(result);
        ptr = command;
        recursive = 0;

        array_push(result, &p->base);

block:
        __block(ptr, &start, &end);
        sub = end - start;
special_case:
        switch (sub) {
                case -1:
                        if(*start == '/' && *end == '/') {
                                recursive = 1;
                                ptr = end + 1;
                                goto block;
                        } else {
                                if(result->len == 1) {
                                        ref = array_get(result, 0);
                                        if(ref == &p->base) {
                                                array_clear(result);
                                        }
                                }
                                goto finish;
                        }
                case 0:
                        switch (*start) {
                                case '*':
                                        if(recursive) {
                                                __get_all_children_recursive(result);
                                        } else {
                                                __get_all_children(result);
                                        }
                                        recursive = 0;
                                        ptr = end + 1;
                                        goto block;
                                case '.':
                                        recursive = 0;
                                        ptr = end + 1;
                                        goto block;
                                case '/':
                                        recursive = 1;

                                        ptr = end;
                                        goto block;
                                case '@':
                                        array_clear(result);
                                        goto finish;
                                default:
                                        goto detail_case;
                        }
                case 1:
                        if(memcmp(start, "..", 2) == 0) {
                                recursive = 0;
                                __move_to_parent(result);

                                ptr = end + 1;
                                goto block;
                        } else if(memcmp(start, "@*", 2) == 0) {
                                if(recursive) {
                                        __get_all_children_recursive(result);
                                }
                                __fetch_all_attribute(result);
                                goto finish;
                        } else {
                                goto detail_case;
                        }
                default:
                        goto detail_case;
        }

detail_case:
        if(*start == '@') {
                if(*(end + 1) != '\0') {
                        array_clear(result);
                } else {
                        if(recursive) {
                                __get_all_children_recursive(result);
                        }
                        __fetch_attribute_name(result, start + 1, end - start);
                        goto finish;
                }
        } else {
                __block_name(start, end, &mid);
                if(*mid) {
                        if(mid == start && *mid == '*') {
                                if(recursive) {
                                        __get_all_children_recursive(result);
                                } else {
                                        __get_all_children(result);
                                }
                        } else {
                                if(recursive) {
                                        __get_all_children_has_name_recursive(result,
                                                start, mid - start + 1);
                                } else {
                                        __get_all_children_has_name(result,
                                                start, mid - start + 1);
                                }

                        }

                        mid++;
                filter_attribute:
                        __block_attribute(mid, end, &name[0], &name[1],
                                &value[0], &value[1]);
                        if(*name[0]) {
                                if(name[1] == name[0] && *name[0] == '*') {
                                        if(value[0] <= value[1]) {
                                                __keep_node_has_at_least_one_attribute_with_value(result,
                                                        value[0], value[1] - value[0] + 1);
                                        } else {
                                                __keep_node_has_at_least_one_attribute(result);
                                        }
                                } else {
                                        if(value[0] <= value[1]) {
                                                __keep_node_has_attribute_with_value(
                                                        result,
                                                        name[0], name[1] - name[0] + 1,
                                                        value[0], value[1] - value[0] + 1
                                                );
                                        } else {
                                                __keep_node_has_attribute(result,
                                                        name[0], name[1] - name[0] + 1);
                                        }
                                }
                                mid = value[1] + 2;
                                goto filter_attribute;
                        }
                }
                recursive = 0;
        }
        ptr = end + 1;
        goto block;

finish:
        ;
}
