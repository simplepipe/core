#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <kernel/hash_table.h>

static const int g_size[] = {
        2, 5, 11, 23, 47, 97, 197, 397, 797, 1597,
        3203, 6421, 7919, 16069, 33391, 69313, 150151
};

static inline size_t __hash(const char* key, const size_t key_size,
        const size_t cache_size)
{
        size_t hash = 5381;
        int i;
        for(i = 0; i < key_size; i++) {
                hash = ((hash << 5) + hash) + key[i];
        }
        return hash % cache_size;
}

struct cell;
struct entry;

static struct cell *__cell_new(const char *key, const unsigned key_len,
        struct ref *ref, struct entry *entry);
static void __cell_free(struct cell *p);

static struct entry *__entry_new(const int size, const int index);
static void __entry_free(struct entry *p);

static struct cell *__hash_table_get(struct hash_table *p,
        const char *key, const unsigned key_len);

static struct entry *__hash_table_get_insert_entry(struct hash_table *p);

static void hash_table_free(const struct ref *ref);

struct cell {
        struct list_head head_block;
        struct list_head head_iterator;

        const char *key;
        const unsigned key_len;

        struct ref *ref;
        struct entry *entry;

        unsigned index;
};

struct entry {
        struct list_head head_table;
        struct list_head *blocks;
        const int size;
        int count;
        const int index;
};

static struct cell *__cell_new(const char *key, const unsigned key_len,
        struct ref *ref, struct entry *entry)
{
        size_t slot;
        char *buf;

        struct cell *p = malloc(sizeof(struct cell) + key_len + 1);
        list_head_init(&p->head_block);
        list_head_init(&p->head_iterator);

        p->index = 0;

        buf = (char *)(p + 1);
        memcpy(buf, key, key_len);
        buf[key_len] = '\0';

        p->key = buf;
        *(unsigned *)&p->key_len = key_len;

        ref_inc(ref);
        p->ref = ref;
        p->entry = entry;
        entry->count++;

        slot = __hash(key, key_len, entry->size);
        list_add_tail(&p->head_block, &entry->blocks[slot]);

        return p;
}

static void __cell_free(struct cell *p)
{
        struct entry *entry = p->entry;

        ref_dec(p->ref);
        list_del(&p->head_block);
        list_del(&p->head_iterator);
        free(p);

        entry->count--;
        if(!entry->count) {
                __entry_free(entry);
        }
}

static struct entry *__entry_new(const int size, const int index)
{
        int i;

        struct entry *p = malloc(sizeof(struct entry));
        list_head_init(&p->head_table);
        p->count = 0;
        *(int *)&p->size = size;
        *(int *)&p->index = index;

        p->blocks = malloc(sizeof(struct list_head) * size);
        for(i = size - 1; i >= 0; --i) {
                list_head_init(&p->blocks[i]);
        }

        return p;
}

static void __entry_free(struct entry *p)
{
        int i;
        struct list_head *head;
        struct cell *cell;
        p->count = 0; /* prevent freed from cell */
        for(i = p->size - 1; i >= 0; --i) {
                list_while_not_singular(head, &p->blocks[i]) {
                        cell = cast(head, struct cell, head_block);
                        __cell_free(cell);
                }
        }
        list_del(&p->head_table);
        free(p->blocks);
        free(p);
}

struct hash_table *hash_table_new()
{
        return hash_table_init(malloc(sizeof(struct hash_table)));
}

struct hash_table *hash_table_init(struct hash_table *p)
{
        p->base = (struct ref){hash_table_free, 1};
        list_head_init(&p->entries);
        list_head_init(&p->iterators);
        return p;
}

void hash_table_release(struct hash_table *p)
{
        struct list_head *h;
        struct entry *e;
        list_while_not_singular(h, &p->entries) {
                e = cast(h, struct entry, head_table);
                __entry_free(e);
        }
}

static void hash_table_free(const struct ref *ref)
{
        struct hash_table *p = cast(ref, struct hash_table, base);
        hash_table_release(p);
        free(p);
}

static struct entry *__hash_table_get_insert_entry(struct hash_table *p)
{
        struct list_head *head;
        struct entry *entry;
        int index = 0;

        head = p->entries.prev;
        if(head == &p->entries) {
                entry = __entry_new(g_size[0], index);
                list_add_tail(&entry->head_table, &p->entries);
                goto finish;
        }

        entry = cast(head, struct entry, head_table);
        if(entry->count >= entry->size * 0.7f) {
                index = entry->index + 1;
                if(index < sizeof(g_size) / sizeof(g_size[0])) {
                        entry = __entry_new(g_size[index], index);
                        list_add_tail(&entry->head_table, &p->entries);
                } else {
                        entry = __entry_new(entry->size * 2, index);
                        list_add_tail(&entry->head_table, &p->entries);
                }
        }

finish:
        return entry;
}

void hash_table_set(struct hash_table *p, struct ref *ref,
        const void *key, const unsigned key_len)
{
        struct cell *cell;
        struct entry *entry;
        struct list_head *list_cell;
        size_t slot;

        cell = __hash_table_get(p, key, key_len);

        if(cell) {
                if(cell->ref == ref) {
                        goto finish;
                }

                __cell_free(cell);
        }

        entry = __hash_table_get_insert_entry(p);
        cell = __cell_new(key, key_len, ref, entry);
        list_add_tail(&cell->head_iterator, &p->iterators);
finish:
        ;
}

static struct cell *__hash_table_get(struct hash_table *p, const char *key,
        const unsigned key_len)
{
        struct list_head *head_entry, *head_cell;
        size_t slot;
        struct entry *entry, *entry_last;
        struct list_head *list_cell;
        struct cell *cell;

        head_entry = &p->entries;
next_entry:
        head_entry = head_entry->next;
        if(head_entry == &p->entries) {
                cell = NULL;
                goto finish;
        }

        entry = cast(head_entry, struct entry, head_table);
        slot = __hash(key, key_len, entry->size);
        list_cell = &entry->blocks[slot];

        head_cell = list_cell;
next_cell:
        head_cell = head_cell->next;
        if(head_cell == list_cell)
                goto next_entry;

        cell = cast(head_cell, struct cell, head_block);
        if(cell->key_len == key_len
                && memcmp(cell->key, key, key_len) == 0) {
                goto found;
        }

        goto next_cell;

found:
        entry_last = cast(p->entries.prev, struct entry, head_table);
        if(entry_last == entry)
                goto finish;

        list_del(&cell->head_block);
        slot = __hash(key, key_len, entry_last->size);
        list_cell = &entry_last->blocks[slot];
        list_add_tail(&cell->head_block, list_cell);
        cell->entry = entry_last;

        entry_last->count++;
        entry->count--;
        if(!entry->count) {
                list_del_init(&entry->head_table);
                __entry_free(entry);
        }
finish:
        return cell;
}

struct ref *hash_table_get(struct hash_table *p, const void *key,
        const unsigned key_len)
{
        struct cell *cell = __hash_table_get(p, key, key_len);
        if(cell) {
                return cell->ref;
        } else {
                return NULL;
        }
}

void hash_table_remove(struct hash_table *p, const void *key,
        const unsigned key_len)
{
        struct cell *cell = __hash_table_get(p, key, key_len);
        if(cell) {
                __cell_free(cell);
        }
}

struct list_head *hash_table_get_iterator(struct hash_table *p)
{
        return p->iterators.next;
}

struct ref *hash_table_iterator_next(struct hash_table *p, struct list_head **pit)
{
        struct ref *ref = NULL;
        struct list_head *it = *pit;

        if(it != &p->iterators) {
                ref = cast(it, struct cell, head_iterator)->ref;
                *pit = it->next;
        }
        return ref;
}
