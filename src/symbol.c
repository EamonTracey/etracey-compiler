#include <stdio.h>
#include <stdlib.h>

#include "symbol.h"

extern int verb;

struct symbol *symbol_create(symbol_t kind, struct type *type, char *name) {
    struct symbol *symbol = (struct symbol *)malloc(sizeof(struct symbol));

    if (symbol == NULL)
        return NULL;

    symbol->kind = kind;
    symbol->type = type;
    symbol->name = name;

    return symbol;
}

void symbol_print(struct symbol *s) {
    if (!verb)
        return;

    fprintf(stdout, "%s resolves to ", s->name);

    switch (s->kind) {
    case SYMBOL_LOCAL:
        fprintf(stdout, "local %d\n", s->which);
        break;
    case SYMBOL_PARAM:
        fprintf(stdout, "param %d\n", s->which);
        break;
    case SYMBOL_GLOBAL:
        fprintf(stdout, "global %s\n", s->name);
        break;
    }
}

const char *symbol_codegen(struct symbol *s) {
    switch (s->kind) {
    case SYMBOL_LOCAL:
    case SYMBOL_PARAM:
    case SYMBOL_GLOBAL:
        break;
    }

    return "";
}
