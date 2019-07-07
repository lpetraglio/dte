#include <stdlib.h>
#include <string.h>
#include "hashset.h"
#include "../util/hash.h"
#include "../util/str-util.h"
#include "../util/xmalloc.h"

static void alloc_table(HashSet *set, size_t size)
{
    set->table_size = size;
    set->table = xnew0(HashSetEntry*, size);
    set->grow_at = size - (size / 4); // 75% load factor (size * 0.75)
}

void hashset_init(HashSet *set, size_t size, bool icase)
{
    if (size < 8) {
        size = 8;
    }

    // Accomodate the 75% load factor in the table size, to allow filling
    // the set to the requested size without needing to rehash()
    size += size / 3;

    // Round up the allocation to the next power of 2, to allow using
    // simple bitwise ops (instead of modulo) in get_slot()
    size = round_size_to_next_power_of_2(size);

    alloc_table(set, size);
    set->nr_entries = 0;

    if (icase) {
        set->hash = fnv_1a_32_hash_icase;
        set->equal = mem_equal_icase;
    } else {
        set->hash = fnv_1a_32_hash;
        set->equal = mem_equal;
    }
}

void hashset_free(HashSet *set)
{
    for (size_t i = 0, n = set->table_size; i < n; i++) {
        HashSetEntry *h = set->table[i];
        while (h) {
            HashSetEntry *next = h->next;
            free(h);
            h = next;
        }
    }
    free(set->table);
}

bool hashset_contains(const HashSet *set, const char *str, size_t str_len)
{
    uint32_t hash = set->hash(str, str_len);
    HashSetEntry *h = set->table[hash % set->table_size];
    while (h) {
        if (str_len == h->str_len && set->equal(str, h->str, str_len)) {
            return true;
        }
        h = h->next;
    }
    return false;
}

static size_t get_slot(const HashSet *set, const char *str, size_t str_len)
{
    const uint32_t hash = set->hash(str, str_len);
    return (size_t)hash & (set->table_size - 1);
}

static void rehash(HashSet *set, size_t newsize)
{
    size_t oldsize = set->table_size;
    HashSetEntry **oldtable = set->table;
    alloc_table(set, newsize);
    for (size_t i = 0; i < oldsize; i++) {
        HashSetEntry *e = oldtable[i];
        while (e) {
            HashSetEntry *next = e->next;
            const size_t slot = get_slot(set, e->str, e->str_len);
            e->next = set->table[slot];
            set->table[slot] = e;
            e = next;
        }
    }
    free(oldtable);
}

void hashset_add(HashSet *set, const char *str, size_t str_len)
{
    const size_t slot = get_slot(set, str, str_len);
    HashSetEntry *h = xmalloc(sizeof(*h) + str_len);
    h->next = set->table[slot];
    h->str_len = str_len;
    memcpy(h->str, str, str_len);
    set->table[slot] = h;

    if (++set->nr_entries > set->grow_at) {
        rehash(set, set->table_size << 1);
    }
}

void hashset_add_many(HashSet *set, char **strings, size_t nstrings)
{
    for (size_t i = 0; i < nstrings; i++) {
        const char *str = strings[i];
        const size_t str_len = strlen(str);
        hashset_add(set, str, str_len);
    }
}
