#include <stdlib.h>
#include <string.h>
#include <math/math.h>
#include <collada/int_array.h>

static void __int_array_free(const struct ref *ref);

struct int_array *int_array_new()
{
        return int_array_init(malloc(sizeof(struct int_array)));
}

struct int_array *int_array_init(struct int_array *p)
{
        p->base = (struct ref){__int_array_free, 1};
        p->ptr = NULL;
        p->len = 0;
        p->reserved = 0;
        return p;
}

void int_array_release(struct int_array *p)
{
        if(p->ptr) {
                free(p->ptr);
                p->ptr = NULL;
        }
}

static void __int_array_free(const struct ref *ref)
{
        struct int_array *p = cast(ref, struct int_array, base);
        int_array_release(p);
        free(p);
}

void int_array_reserve(struct int_array *p, const unsigned len)
{
        if(p->reserved >= len) return;

        p->ptr = realloc(p->ptr, sizeof(signed) * len);
        p->reserved = len;
}

signed int_array_add_string(struct int_array *p, const char *buf)
{
        const char *ptr = buf;
        char * endptr;
        signed max = 0;
next:
        if(!ptr || !*ptr) goto end;

        switch (*ptr) {
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9':
                case '0': case '+': case '-':
                        int_array_reserve(p, p->len + 1);
                        p->ptr[p->len] = strtol(ptr, &endptr, 10);
                        max = MAX(p->ptr[p->len], max);
                        p->len++;
                        ptr = endptr;
                        goto next;
                default:
                        ptr++;
                        goto next;
        }

end:
        return max;
}

void int_array_add_stream(struct int_array *p, const signed * restrict buf,
        const unsigned len)
{
        int_array_reserve(p, p->len + len);
        memcpy(p->ptr + p->len, buf, sizeof(float) * len);
        p->len += len;
}

void int_array_add_int(struct int_array *p, const signed f)
{
        int_array_reserve(p, p->len + 1);
        p->ptr[p->len] = f;
        p->len++;
}

void int_array_assign(struct int_array **p, struct int_array *a)
{
        if(a) {
                ref_inc(&a->base);
        }
        if(*p) {
                ref_dec(&(*p)->base);
        }
        *p = a;
}
