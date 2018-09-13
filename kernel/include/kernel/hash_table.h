#ifndef __kernel_hash_table_h
#define __kernel_hash_table_h

#include <kernel/ref.h>
#include <kernel/list.h>

struct hash_table {
        struct ref base;

        struct list_head entries;
        struct list_head iterators;
};

struct hash_table *hash_table_new();
struct hash_table *hash_table_init(struct hash_table *p);
void hash_table_release(struct hash_table *p);

struct ref *hash_table_get(struct hash_table *p, const void *key,
        const unsigned key_len);

void hash_table_set(struct hash_table *p, struct ref *ref,
        const void *key, const unsigned key_len);

void hash_table_remove(struct hash_table *p, const void *key,
        const unsigned key_len);

struct list_head *hash_table_get_iterator(struct hash_table *p);
struct ref *hash_table_iterator_next(struct hash_table *p, struct list_head **it);

#endif
