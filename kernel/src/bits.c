#include <stdlib.h>
#include <string.h>
#include <kernel/bits.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

static const unsigned g_range = 8 * sizeof(unsigned);

static void __bits_free(const struct ref *ref);
static void __bits_reserve(struct bits *p, unsigned len);

struct bits *bits_new(const unsigned ordered)
{
        return bits_init(malloc(sizeof(struct bits)), ordered);
}

struct bits *bits_init(struct bits *p, const unsigned ordered)
{
        p->base = (struct ref){__bits_free, 1};
        p->ptr = NULL;
        p->len = 0;
        p->reserved = 0;
        p->ordered = ordered;
        return p;
}

void bits_release(struct bits *p)
{
        if(p->ptr) {
                free(p->ptr);
                p->ptr = NULL;
        }
}

static void __bits_free(const struct ref *ref)
{
        struct bits *p = cast(ref, struct bits, base);
        bits_release(p);
        free(p);
}

static void __bits_reserve(struct bits *p, unsigned len)
{
        unsigned r = MAX(1, len / g_range);
        if(p->reserved < r) {
                p->ptr = realloc(p->ptr, sizeof(unsigned) * r);
                p->reserved = r;
        }
}

static inline void __bits_set_zero(struct bits *p, const unsigned id)
{
        // unsigned *c;
        // unsigned pos;

        // c = p->ptr + id / g_range;
        // pos = id % g_range;

        *(p->ptr + id / g_range) &= ~(1u << (id % g_range));
}

static inline void __bits_set_one(struct bits *p, const unsigned id)
{
        *(p->ptr + id / g_range) |= (1u << (id % g_range));
}

void bits_add(struct bits *p, unsigned b)
{
        __bits_reserve(p, p->len + 1);
        p->len++;
        if(b) {
                __bits_set_one(p, p->len - 1);
        } else {
                __bits_set_zero(p, p->len - 1);
        }
}

static inline void __bits_set(struct bits *p, unsigned b, unsigned id)
{
        if(b) {
                __bits_set_one(p, id);
        } else {
                __bits_set_zero(p, id);
        }
}

void bits_set(struct bits *p, unsigned b, unsigned id)
{
        if(id >= p->len) return;

        __bits_set(p, b, id);
}

static inline unsigned __bits_get(struct bits *p, unsigned id)
{
        return (*(p->ptr + id / g_range) & (1u << (id % g_range))) ? 1 : 0;
}

unsigned bits_get(struct bits *p, unsigned id)
{
        if(id >= p->len) return 0;

        return __bits_get(p, id);
}

void bits_zero(struct bits *p)
{
        if(p->reserved) {
                memset(p->ptr, 0, sizeof(unsigned) * p->reserved);
        }
}

void bits_remove(struct bits *p, unsigned id)
{
        const unsigned len = p->len;

        if(id >= len) return;

        if(p->ordered) {
                while(id < len - 1) {
                        __bits_set(p, __bits_get(p, id + 1), id);
                        id++;
                }
        } else {
                __bits_set(p, __bits_get(p, len - 1), id);
        }
        p->len--;
}

void bits_clear(struct bits *p)
{
        p->len = 0;
}
