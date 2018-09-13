#include <stdlib.h>
#include <kernel/number.h>

static void number_free(const struct ref *ref);

struct number *number_new()
{
        return number_init(malloc(sizeof(struct number)));
}

struct number *number_init(struct number *p)
{
        p->base = (struct ref){number_free, 1};
        p->internal = 0;
        return p;
}

void number_release(struct number *p)
{

}

static void number_free(const struct ref *ref)
{
        struct number *p = cast(ref, struct number, base);
        number_release(p);
        free(p);
}
