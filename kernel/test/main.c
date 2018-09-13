#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <unistd.h>
#include <kernel/def.h>
#include <kernel/array.h>
#include <kernel/hash_table.h>
#include <kernel/string.h>
#include <kernel/number.h>
#include <kernel/io.h>
#include <kernel/thread.h>
#include <kernel/utf8.h>
#include <kernel/bits.h>

static void __work_1(const struct ref *ref)
{
        printf("1\n");
}

static void __work_2(const struct ref *ref)
{
        printf("2\n");
}

int main(int argc, char **argv)
{
        struct bits *b = bits_new(1);

        bits_add(b, 1);
        bits_add(b, 0);
        bits_add(b, 1);
        bits_add(b, 0);
        bits_add(b, 0);

        printf("%d %d %d %d %d\n",
                bits_get(b, 0),
                bits_get(b, 1),
                bits_get(b, 2),
                bits_get(b, 3),
                bits_get(b, 4));

        bits_set(b, 0, 2);

        printf("%d %d %d %d %d\n",
                bits_get(b, 0),
                bits_get(b, 1),
                bits_get(b, 2),
                bits_get(b, 3),
                bits_get(b, 4));

        bits_set(b, 1, 3);

        printf("%d %d %d %d %d\n",
                bits_get(b, 0),
                bits_get(b, 1),
                bits_get(b, 2),
                bits_get(b, 3),
                bits_get(b, 4));

        bits_zero(b);

        printf("%d %d %d %d %d\n",
                bits_get(b, 0),
                bits_get(b, 1),
                bits_get(b, 2),
                bits_get(b, 3),
                bits_get(b, 4));

        ref_dec(&b->base);

        // struct hash_table *h = hash_table_new();
        // // struct array *p = array_new();
        // struct string *s = string_new();
        //
        // string_cat(s, STRING_CAT_LITERAL("           Hello World"),
        //         STRING_CAT_I8(-126), STRING_CAT_I8(0),
        //         STRING_CAT_I8(-1), STRING_CAT_U16(65535),
        //         STRING_CAT_I8(-1), STRING_CAT_U32(2147483649),
        //         STRING_CAT_I8(-1), STRING_CAT_U64(ULONG_MAX + 1),
        //         STRING_CAT_LITERAL("Goodbye 123 --      "), NULL);
        //
        // printf("%s|\n", s->ptr);
        // string_trim_utf8(s);
        // printf("%s|\n", s->ptr);
        //
        // struct number *n = number_new();
        // number_set(n, 1);
        // if(number_set(n, 2.5) > 0) {
        //         printf("%u\n", number_to(n, u8));
        // }
        //
        // // struct file *f = file_open("local://res/sub/file.txt","w");
        // // file_write(f, KEYSIZE_L("Hello World"));
        // //
        // //
        // // struct string *ss = file_to_string("local://res/sub/file.txt");
        // // printf("%s\n", ss->ptr);
        //
        // struct thread_pool *pool = thread_pool_new(4);
        // // for(int i = 0; i < 2000; ++i) {
        // //         thread_pool_add(pool, NULL, __work_1);
        // //         thread_pool_add(pool, NULL, __work_2);
        // // }
        // ref_dec(&pool->base);
        // ref_dec(&n->base);
        // ref_dec(&s->base);
        // // ref_dec(&p->base);
        // ref_dec(&h->base);
        // // ref_dec(&ss->base);
        // // ref_dec(&f->base);
        //
        // sleep(3);
        return 0;
}
