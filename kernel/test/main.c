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


int main(int argc, char **argv)
{
        struct string *s = string_new();
        string_cat_chars(s, KEYSIZE_L("Hello World | Hello World | Hello"));
        string_replace(s, KEYSIZE_L("Hello"), KEYSIZE_L("Hi"));
        printf("%s\n", s->ptr);
        ref_dec(&s->base);
        return 0;
}
