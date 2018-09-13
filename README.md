# Core guidelines
* [How atomic reference counting works](how-atomic-reference-counting-works)
* [How to create an object](how-to-create-an-object)
* [How to use an object](how-to-use-an-object)
* [How to build each platform](how-to-build-each-platform)
* [How to clean each platform](how-to-clean-each-platform)
&NewLine;
&NewLine;
#### How atomic reference counting works
*ref.h*
```C
#include <stddef.h>

struct ref {
        void(*free)(const struct ref *);
        volatile signed count;
};

static inline void
ref_inc(const struct ref *ref)
{
        __sync_add_and_fetch((volatile signed *)&ref->count, 1);
}

static inline void
ref_dec(const struct ref *ref)
{
        if (__sync_sub_and_fetch((volatile signed *)&ref->count, 1) == 0) {
                ref->free(ref);
        }
}

#define cast(ptr, type, member) \
    ((type *) ((char *)(ptr) - offsetof(type, member)))

#define cast_null(ptr, type, member) \
        ((type *) (ptr ? ((char *)(ptr) - offsetof(type, member)) : NULL))
```
&NewLine;
#### How to create an object
*object.h*
```C
#include <kernel/ref.h>

struct object {
        struct ref base;
        char *name;
};
struct object *object_new(const char *name);
struct object *object_init(struct object *p);
void object_release(struct object *p);
```
*object.c*
```C
#include <stdlib.h>
#include <string.h>
#include "object.h"

static void __object_free(const struct ref *ref);

struct object *object_new(const char *name)
{
        unsigned len = strlen(name);
        /*
         * allocate object using malloc routine
         */
        struct object *p = object_init(malloc(sizeof(struct object)));
        p->name = malloc(len);
        memcpy(p->name, name, len);
        return p;
}

struct object *object_init(struct object *p)
{
        /*
         * Init object with reference count = 1
         */
        p->base.ref = (struct ref){__object_free, 1};
        p->name = NULL;
        return p;
}

void object_release(struct object *p)
{
        if(p->custom_data) {
                free(p->custom_data);
                p->custom_data = NULL;
        }
}

void __object_free(const struct ref *ref)
{
        struct object *p = cast(ref, struct object, base);
        object_release(p);
        /*
         * deallocate object using free routine
         */
        free(p);
}
```
&NewLine;
#### How to use an object
*main.c*
```C
#include <stdio.h>
#include "object.h"

int main(int argc, char **argv)
{
        /*
         * allocate object
         */
        struct object *p = object_new("Hello World");
        /*
         * print object name
         */
        printf("%s\n", p->name);
        /*
         * decrease object reference counts by 1
         */
        ref_dec(&p->base);
}
```
&NewLine;
#### How to build each platform
```
bash make.sh linux
bash make.sh droid
bash make.sh ios
bash make.sh osx
bash make.sh web
```
&NewLine;
#### How to clean each platform
```
bash clean.sh linux
bash clean.sh droid
bash clean.sh ios
bash clean.sh osx
bash clean.sh web
```
