#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>
#include <kernel/utf8.h>
#include <kernel/io.h>
#include <xml/xml.h>

static char *null = (char[2]){'\0', '\0'};

static inline void __set_reverse(const unsigned n,
        char **restrict l, char * restrict r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] = &r[n - i - 1];
        }
}

static void __xml_context_free(const struct ref *ref);

struct xml_context *xml_context_new()
{
        return xml_context_init(malloc(sizeof(struct xml_context)));
}

struct xml_context *xml_context_init(struct xml_context *p)
{
        p->base = (struct ref){__xml_context_free, 1};
        p->root = NULL;
        p->data = NULL;

        return p;
}

void xml_context_release(struct xml_context *p)
{
        if(p->root) {
                ref_dec(&p->root->base);
                p->root = NULL;
        }
        if(p->data) {
                ref_dec(&p->data->base);
                p->data = NULL;
        }
}

static void __xml_context_free(const struct ref *ref)
{
        struct xml_context *p = cast(ref, struct xml_context, base);
        xml_context_release(p);
        free(p);
}

enum {
        BLOCK_COMMENT,
        BLOCK_PROLOG,
        BLOCK_NODE,
        BLOCK_END_NODE,
        BLOCK_FULL_NODE,
        BLOCK_TEXT
};

static void __get_block(char * str, char **start,
        char **end, i8 *type)
{
        u8 width;
        char *last;
        char *ptr = str;
        unsigned last_len;
        signed stack_literal;
        char stack_mark;

        *start = &null[1];
        *end = &null[0];
        *type = -1;

        last = ">";
        last_len = 1;

head:
        switch (*ptr) {
                case ' ': case '\t': case '\n':
                case '\v': case '\f': case '\r':
                        ptr++;
                        goto head;
                case '<':
                        break;
                case '\0':
                        goto finish;
                default:
                        width = utf8_width(ptr);
                        ptr += width;
                        goto head;
        }

        if(ptr > str) {
                *start = str;
                *end = ptr - 1;
                *type = BLOCK_TEXT;
                goto finish;
        }

        if(strncmp(ptr, "<!--", 4) == 0) {
                *start = ptr;
                ptr += 4;
                last = "-->";
                last_len = 3;
                *type = BLOCK_COMMENT;
        } else if(strncmp(ptr, "<?xml", 5) == 0) {
                *start = ptr;
                ptr += 5;
                last = "?>";
                last_len = 2;
                *type = BLOCK_PROLOG;
        } else {
                *start = ptr;
                ptr++;
                switch (*ptr) {
                        case ' ': case '\t': case '\n':
                        case '\v': case '\f': case '\r':
                                *type = -1;
                                *start = &null[1];
                                *end = &null[0];
                                goto finish;
                        case '/':
                                *type = BLOCK_END_NODE;
                                break;
                        default:
                                *type = BLOCK_NODE;
                                break;
                }
        }

        stack_literal = 0;
        stack_mark = 0;
tail:
        switch (*type) {
                case BLOCK_COMMENT:
                        if(strncmp(ptr, last, last_len) == 0) {
                                *end = ptr + last_len - 1;
                        } else {
                                if(*ptr) {
                                        width = utf8_width(ptr);
                                        ptr += width;
                                        goto tail;
                                } else {
                                        *start = &null[1];
                                        *end = &null[0];
                                        *type = -1;
                                }
                        }
                        break;
                default:
                        if(!stack_literal) {
                                switch (*ptr) {
                                        case '\'': case '\"':
                                                stack_literal = 1;
                                                stack_mark = *ptr;
                                                ptr++;
                                                goto tail;
                                        default:
                                                if(strncmp(ptr, last, last_len) == 0) {
                                                        *end = ptr + last_len - 1;
                                                        if(*type == BLOCK_NODE) {
                                                                if(*(*end - 1) == '/') {
                                                                        *type = BLOCK_FULL_NODE;
                                                                }
                                                        }
                                                } else {
                                                        if(*ptr) {
                                                                width = utf8_width(ptr);
                                                                ptr += width;
                                                                goto tail;
                                                        } else {
                                                                *start = &null[1];
                                                                *end = &null[0];
                                                                *type = -1;
                                                        }
                                                }
                                                break;
                                }
                        } else {
                                if(*ptr == stack_mark) {
                                        stack_literal = 0;
                                        ptr++;
                                        goto tail;
                                } else {
                                        if(*ptr) {
                                                width = utf8_width(ptr);
                                                ptr += width;
                                                goto tail;
                                        } else {
                                                *start = &null[1];
                                                *end = &null[0];
                                                *type = -1;
                                        }
                                }
                        }
                        break;
        }
finish:
        ;
}

static struct xml_node *__node(char *start, char *end,
        char *value_start, char *value_end)
{
        struct xml_node *p = NULL;
        char *ptr_prev, *ptr;
        char *name[2];
        char *value[2];
        char value_mark;
        u8 width;

        ptr = start;
        ptr_prev = ptr;
        ptr++;

        __set_reverse(2, name, null);
        __set_reverse(2, value, null);

name:
        switch (*ptr) {
                case ' ': case '\t': case '\n':
                case '\v': case '\f': case '\r':
                case '\0': case '/':
                        name[0] = start + 1;
                        name[1] = ptr - 1;
                        value[0] = value_start;
                        value[1] = value_end;

                        p = xml_node_new_mask(name[0], name[1] - name[0] + 1,
                                value[0], value[1] - value[0] + 1);
                        if(*ptr == '\0') {
                                goto finish;
                        }
                        *ptr = '\0';
                        ptr_prev = ptr;
                        ptr++;
                        break;
                case '=':
                        goto finish;
                default:
                        ptr_prev = ptr;
                        width = utf8_width(ptr);
                        ptr += width;
                        goto name;
        }

search_attribute:
        __set_reverse(2, name, null);
        __set_reverse(2, value, null);

        switch (*ptr) {
                case ' ': case '\t': case '\n':
                case '\v': case '\f': case '\r':
                        ptr_prev = ptr;
                        ptr++;
                        goto search_attribute;
                case '/': case '=': case '\0':
                        goto finish;
                default:
                        break;
        }

        name[0] = name[1] = ptr;
read_attribute_name:
        switch (*ptr) {
                case ' ': case '\t': case '\n':
                case '\v': case '\f': case '\r':
                case '=':
                        name[1] = ptr_prev;
                        *ptr = '\0';
                        ptr++;
                        break;
                case '/': case '\0':
                        goto finish;
                default:
                        ptr_prev = ptr;
                        width = utf8_width(ptr);
                        ptr += width;
                        goto read_attribute_name;
        }

search_value:
        switch (*ptr) {
                case '\'': case '\"':
                        value[0] = value[1] = ptr + 1;
                        value_mark = *ptr;
                        ptr_prev = ptr;
                        ptr++;
                        break;
                case '/': case '=': case '\0':
                        goto finish;
                default:
                        ptr_prev = ptr;
                        width = utf8_width(ptr);
                        ptr += width;
                        goto search_value;
        }

read_value:
        switch (*ptr) {
                case '\0':
                        goto finish;
                default:
                        if(*ptr == value_mark) {
                                value[1] = ptr - 1;
                                *ptr = '\0';
                                xml_node_add_attribute_mask(p,
                                        name[0], name[1] - name[0] + 1,
                                        value[0], value[1] - value[0] + 1);
                                ptr++;
                                goto search_attribute;
                        } else {
                                ptr_prev = ptr;
                                width = utf8_width(ptr);
                                ptr += width;
                                goto read_value;
                        }
        }

finish:
        return p;
}

static void __parse(struct xml_context *p, char *str)
{
        char *ptr;
        char *start, *end;
        char *start_prev, *end_prev;
        i8 type, type_prev;
        struct xml_node *n;

        start = end = start_prev = end_prev = NULL;
        type = type_prev = -1;

        ptr = str;
get:
        __get_block(ptr, &start, &end, &type);

        switch (type) {
                case BLOCK_PROLOG:
                        ptr = end + 1;
                        goto get;
                case BLOCK_FULL_NODE:
                        if(type_prev == BLOCK_NODE) {
                                *start_prev = '\0';
                                *end_prev = '\0';
                                n = __node(start_prev, end_prev, &null[1], &null[0]);
                                if(!p->root) {
                                        p->root = n;
                                } else {
                                        xml_node_add_node(p->root, n);
                                        ref_dec(&n->base);
                                        p->root = n;
                                }
                                type_prev = -1;
                                start_prev = NULL;
                                end_prev = NULL;
                        }
                        *start = '\0';
                        *end = '\0';
                        n = __node(start, end, &null[1], &null[0]);
                        if(!p->root) {
                                p->root = n;
                        } else {
                                xml_node_add_node(p->root, n);
                                ref_dec(&n->base);
                                ptr = end + 1;
                                goto get;
                        }
                        break;
                case BLOCK_NODE:
                        if(type_prev == BLOCK_NODE) {
                                *start_prev = '\0';
                                *end_prev = '\0';
                                n = __node(start_prev, end_prev, &null[1], &null[0]);
                                if(!p->root) {
                                        p->root = n;
                                } else {
                                        xml_node_add_node(p->root, n);
                                        ref_dec(&n->base);
                                        p->root = n;
                                }
                        }
                        start_prev = start;
                        end_prev = end;
                        type_prev = type;
                        ptr = end + 1;
                        goto get;
                case BLOCK_TEXT:
                        if(type_prev == BLOCK_NODE) {
                                *start_prev = '\0';
                                *end_prev = '\0';
                                n = __node(start_prev, end_prev, start, end);
                                if(!p->root) {
                                        p->root = n;
                                } else {
                                        xml_node_add_node(p->root, n);
                                        ref_dec(&n->base);
                                        p->root = n;
                                }
                                start_prev = end_prev = NULL;
                                type_prev = -1;
                        }
                        ptr = end + 1;
                        goto get;
                case BLOCK_END_NODE:
                        if(type_prev == BLOCK_NODE) {
                                *start_prev = '\0';
                                *end_prev = '\0';
                                n = __node(start_prev, end_prev, &null[1], &null[0]);
                                if(!p->root) {
                                        p->root = n;
                                } else {
                                        xml_node_add_node(p->root, n);
                                        ref_dec(&n->base);
                                }
                                type_prev = -1;
                                start_prev = NULL;
                                end_prev = NULL;
                        }
                        *start = '\0';
                        *end = '\0';
                        if(p->root && p->root->parent) {
                                p->root = p->root->parent;
                        }
                        ptr = end + 1;
                        goto get;
                case BLOCK_COMMENT:
                        *start = '\0';
                        *end = '\0';
                        ptr = end + 1;
                        goto get;
                default:
                        break;
        }
}

void xml_context_parse(struct xml_context *p, const char *path)
{
        struct string *data;
        xml_context_release(p);

        data = file_to_string(path);
        if(data->len) {
                __parse(p, data->ptr);
        } else {
                ref_dec(&data->base);
                data = NULL;
        }
        p->data = data;
}
