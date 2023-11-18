#include <stddef.h>

#include "hash_table.h"
#include "scope.h"
#include "symbol.h"

struct hash_table *symbol_table = NULL;

void scope_enter() {
    struct hash_table *table = hash_table_create(0, 0);
    table->next = symbol_table;
    symbol_table = table;
}

void scope_exit() {
    struct hash_table *tmp = symbol_table;
    symbol_table = symbol_table->next;
    hash_table_delete(tmp);
}

int scope_level() {
    int level = 0;
    struct hash_table *curr = symbol_table;
    while (curr != NULL) {
        curr = curr->next;
        ++level;
    }
    return level;
}

void scope_bind(const char *name, struct symbol *s) {
    hash_table_insert(symbol_table, name, s);
}

struct symbol *scope_lookup(const char *name) {
    struct hash_table *curr = symbol_table;
    while (curr != NULL) {
        struct symbol *s = hash_table_lookup(curr, name);
        if (s != NULL)
            return s;
        curr = curr->next;
    }
    return NULL;
}

struct symbol *scope_lookup_current(const char *name) {
    return (struct symbol *)hash_table_lookup(symbol_table, name);
}
