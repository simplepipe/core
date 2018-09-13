#ifndef __json_json_h
#define __json_json_h

#include <kernel/string.h>
#include <kernel/array.h>
#include <kernel/hash_table.h>

enum {
        JSON_MAP,
        JSON_ARRAY
};

struct json_node {
        struct ref base;
        signed type;
};

struct json_map {
        struct json_node base;

        struct hash_table *children;
};

struct json_array {
        struct json_node base;

        struct array *children;
};

#endif
