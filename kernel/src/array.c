#include <stdlib.h>
#include <string.h>
#include <kernel/array.h>

static void array_free(const struct ref *ref);

struct array *array_new(const unsigned ordered)
{
        return array_init(malloc(sizeof(struct array)), ordered);
}

struct array *array_init(struct array *p, const unsigned ordered)
{
        p->base = (struct ref){array_free, 1};
        p->ptr = NULL;
        p->len = 0;
        p->reserved = 0;
        p->ordered = ordered;
        return p;
}

void array_release(struct array *p)
{
        int i;
        if(p->ptr) {
                for(i = p->len - 1; i >= 0; --i) {
                        ref_dec(p->ptr[i]);
                }
                free(p->ptr);
                p->ptr = NULL;
                p->len = 0;
                p->reserved = 0;
        }
}

static void array_free(const struct ref *ref)
{
        struct array *p = cast(ref, struct array, base);

        array_release(p);
        free(p);
}

void array_push(struct array *p, struct ref *ref)
{
        ref_inc(ref);
        array_reserve(p, p->len + 1);
        p->ptr[p->len] = ref;
        p->len++;
}

void array_swap(struct array *p, struct array *q)
{
        struct array tmp;
        tmp.ptr = p->ptr;
        tmp.len = p->len;
        tmp.reserved = p->reserved;

        p->ptr = q->ptr;
        p->len = q->len;
        p->reserved = q->reserved;

        q->ptr = tmp.ptr;
        q->len = tmp.len;
        q->reserved = tmp.reserved;
}

void array_reserve(struct array *p, const unsigned len)
{
        if(p->reserved >= len) return;

        p->ptr = realloc(p->ptr, sizeof(struct ref *) * len);
        p->reserved = len;
}

void array_truncate(struct array *p)
{
        if(p->reserved > p->len) {
                p->ptr = realloc(p->ptr, sizeof(struct ref *) * p->len);
                p->reserved = p->len;
        }
}

void array_set(struct array *p, struct ref *ref, const int id)
{
        struct ref *old;

        if(id < 0 || id >= p->len) return;

        old = p->ptr[id];
        p->ptr[id] = ref;

        ref_inc(ref);
        ref_dec(old);
}

struct ref *array_get(struct array *p, const int id)
{
        if(id < 0 || id >= p->len) return NULL;

        return p->ptr[id];
}

void array_remove(struct array *p, const int id)
{
        struct ref *ref;
        int offset;

        if(id < 0 || id >= p->len) return;

        ref = p->ptr[id];

        if(p->ordered) {
                offset = sizeof(struct ref *) * (p->len - id - 1);
                if(offset > 0)
                        memmove(p->ptr + id, p->ptr + id + 1, offset);
        } else {
                p->ptr[id] = p->ptr[p->len - 1];
        }

        p->len--;
        ref_dec(ref);
}

void array_clear(struct array *p)
{
        int i;
        if(p->ptr) {
                for(i = p->len - 1; i >= 0; --i) {
                        ref_dec(p->ptr[i]);
                }
        }
        p->len = 0;
}
