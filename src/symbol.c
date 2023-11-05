#include <stdlib.h>

#include "symbol.h"

struct symbol* symbol_create(symbol_t kind, struct type *type, char *name) {
    struct symbol *symbol = (struct symbol *)malloc(sizeof(struct symbol));

    if (symbol == NULL)
        return NULL;

    symbol->kind = kind;
    symbol->type = type;
    symbol->name = name;
    symbol->which = 0;

    return symbol;
}
