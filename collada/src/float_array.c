#include <stdlib.h>
#include <string.h>
#include <collada/float_array.h>

static void __float_array_free(const struct ref *ref);

struct float_array *float_array_new()
{
        return float_array_init(malloc(sizeof(struct float_array)));
}

struct float_array *float_array_init(struct float_array *p)
{
        p->base = (struct ref){__float_array_free, 1};
        p->ptr = NULL;
        p->len = 0;
        p->reserved = 0;
        return p;
}

void float_array_release(struct float_array *p)
{
        if(p->ptr) {
                free(p->ptr);
                p->ptr = NULL;
        }
}

static void __float_array_free(const struct ref *ref)
{
        struct float_array *p = cast(ref, struct float_array, base);
        float_array_release(p);
        free(p);
}

void float_array_reserve(struct float_array *p, const unsigned len)
{
        if(p->reserved >= len) return;

        p->ptr = realloc(p->ptr, sizeof(float) * len);
        p->reserved = len;
}

void float_array_add_string(struct float_array *p, const char *buf)
{
        const char *ptr = buf;
        char * endptr;
next:
        if(!ptr || !*ptr) goto end;

        switch (*ptr) {
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9':
                case '0': case '+': case '-':
                        float_array_reserve(p, p->len + 1);
                        p->ptr[p->len] = (float)strtod(ptr, &endptr);
                        p->len++;
                        ptr = endptr;
                        goto next;
                default:
                        ptr++;
                        goto next;
        }

end:
        ;
}

void float_array_add_stream(struct float_array *p, const float * restrict buf,
        const unsigned len)
{
        float_array_reserve(p, p->len + len);
        memcpy(p->ptr + p->len, buf, sizeof(float) * len);
        p->len += len;
}

void float_array_add_float(struct float_array *p, const float f)
{
        float_array_reserve(p, p->len + 1);
        p->ptr[p->len] = f;
        p->len++;
}

void float_array_assign(struct float_array **p, struct float_array *a)
{
        if(a) {
                ref_inc(&a->base);
        }
        if(*p) {
                ref_dec(&(*p)->base);
        }
        *p = a;
}
