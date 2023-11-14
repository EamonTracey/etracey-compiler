#include <stdio.h>
#include <stdlib.h>

#include "expr.h"
#include "indent.h"
#include "scope.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"

extern int resolve_errors;
extern int type_errors;

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

    expr_resolve(d->value);
    expr_resolve(d->type->size);

    struct symbol *s = scope_lookup_current(d->name);

    if (s != NULL) {
        if (s->type->kind == TYPE_FUNCTION && d->type->kind != TYPE_FUNCTION) {
            ++resolve_errors;
            fprintf(stdout, "resolve error: function %s cannot be redeclared as type ", s->name);
            type_print(d->type);
            fprintf(stdout, ".\n");
        } else if (s->type->kind != TYPE_FUNCTION && d->type->kind == TYPE_FUNCTION) {
            ++resolve_errors;
            fprintf(stdout, "resolve error: %s with non-function type ", s->name);
            type_print(s->type);
            fprintf(stdout, " cannot be redeclared as a function.\n");
        } else if (s->type->kind != TYPE_FUNCTION && d->type->kind != TYPE_FUNCTION) {
            ++resolve_errors;
            fprintf(stdout, "resolve error: %s was previously declared as type ", d->name);
            type_print(s->type);
            fprintf(stdout, " in same scope.\n");
        } else if (s->funcdef && d->code) {
            ++resolve_errors;
            fprintf(stdout, "resolve error: body of function %s cannot be redefined.\n", s->name);
        } else
            symbol_print(s);
    } else {
        symbol_t kind = scope_level() == 1 ? SYMBOL_GLOBAL : SYMBOL_LOCAL;
        d->symbol = symbol_create(kind, d->type, d->name);
        d->symbol->funcdef = 0;
        scope_bind(d->name, d->symbol);
        symbol_print(d->symbol);
        s = d->symbol;
    }

    // Function body resolution.
    if (d->code) {
        if (s)
            s->funcdef = 1;
        scope_enter();
        param_list_resolve(d->type->params);
        scope_enter();
        stmt_resolve(d->code);
        scope_exit();
        scope_exit();
    } else if (d->type->params) {
        scope_enter();
        param_list_resolve(d->type->params);
        scope_exit();
    }

    decl_resolve(d->next);
}

void decl_typecheck(struct decl *d) {
    if (d == NULL)
        return;

    if (d->value != NULL) {
        struct type *t = expr_typecheck(d->value);
        if (!type_equals(t, d->symbol->type)) {
            /* TODO: error. */
            ++type_errors;
            fprintf(stdout, "type error: type ");
            type_print(t);
            fprintf(stdout, " (");
            expr_print(d->value, 0);
            fprintf(stdout, ") does not match %s's declaration type ", d->name);
            type_print(d->type);
            fprintf(stdout, ".\n");
        }
    }

    if (d->code) {
        if (d->symbol->type->kind != TYPE_FUNCTION) {
            /* TODO: error. */
            ++type_errors;
        }
        stmt_typecheck(d->code);
    }

    decl_typecheck(d->next);
}
