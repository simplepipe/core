#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <kernel/string.h>
#include <kernel/utf8.h>

static void string_free(const struct ref *ref);

struct string *string_new()
{
        return string_init(malloc(sizeof(struct string)));
}

struct string *string_init(struct string *p)
{
        p->base = (struct ref){string_free, 1};
        p->realloc = realloc;
        p->ptr = malloc(1);
        p->ptr[0] = '\0';
        p->len = 0;
        return p;
}

void string_release(struct string *p)
{
        if(p->ptr) {
                free(p->ptr);
                p->ptr = NULL;
        }
}

static void string_free(const struct ref *ref)
{
        struct string *p = cast(ref, struct string, base);

        string_release(p);
        free(p);
}

void string_trim_utf8(struct string *p)
{
        char *start = p->ptr;
        char *end = NULL, *mid = NULL;
        u8 width = utf8_width(start);

        while(*start && isspace(*start)) {
                start += width;
                width = utf8_width(start);
        }

        if(*start) {
                end = start;
                mid = start;

                while(*mid) {
                        if(!isspace(*mid)) {
                                end = mid;
                        }

                        mid += width;
                        width = utf8_width(mid);
                }

                memmove(p->ptr, start, end - start + 1);
                p->len = end - start + 1;
                p->ptr[p->len] = '\0';
        } else {
                p->ptr[0] = '\0';
                p->len = 0;
        }
}

struct string *string_cat_chars(struct string *p,
        const char *s, const unsigned s_len)
{
        p->ptr = p->realloc(p->ptr, p->len + s_len + 1);
        memcpy(p->ptr + p->len, s, s_len);
        p->len += s_len;
        p->ptr[p->len] = '\0';
        return p;
}

struct string *string_cat(struct string *p,
        struct string_cat_parameter *param, ...)
{
        struct string_cat_parameter *pr;

        va_list parg;
        va_start(parg, param);

        pr = param;
        while(pr) {
                if(pr->cat) {
                        pr->cat(pr, p);
                }
                pr = va_arg(parg, struct string_cat_parameter *);
        }
        va_end(parg);
        return p;
}
