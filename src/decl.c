#include <stdio.h>
#include <stdlib.h>

#include "expr.h"
#include "indent.h"
#include "scope.h"
#include "stmt.h"
#include "type.h"

struct decl *decl_create(char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next) {
    struct decl *decl = (struct decl *)malloc(sizeof(struct decl));
    
    if (decl == NULL)
        return NULL;

    decl->name = name;
    decl->type = type;
    decl->value = value;
    decl->code = code;
    decl->next = next;

    decl->symbol = NULL;

    return decl;
}

void decl_print(struct decl *d, int indent) {
    if (d == NULL)
        return;

    indent_print(indent);
    fprintf(stdout, "%s: ", d->name);
    type_print(d->type);

    if (d->value != NULL) {
        fprintf(stdout, " = ");
        expr_print(d->value, 0);
        fprintf(stdout, ";");
    } else if (d->code != NULL) {
        fprintf(stdout, " = ");
        stmt_print(d->code, indent + 1);
    } else {
        fprintf(stdout, ";");
    }

    fprintf(stdout, "\n");

    decl_print(d->next, indent);
}

void decl_resolve(struct decl *d) {
    if (d == NULL)
        return;

    if (scope_lookup_current(d->name) != NULL) {
        fprintf(stderr, "error: previously declared!!!\n");
        return;
    }

    symbol_t kind = scope_level() == 1 ? SYMBOL_GLOBAL : SYMBOL_LOCAL;
    d->symbol  = symbol_create(kind, d->type, d->name);
    scope_bind(d->name, d->symbol);

    // Function declaration.
    if (d->code) {
        scope_enter();
        param_list_resolve(d->type->params);
        stmt_resolve(d->code);
        scope_exit();
    }

    decl_resolve(d->next);
}
