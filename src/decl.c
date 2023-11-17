#include <stdio.h>
#include <stdlib.h>

#include "expr.h"
#include "indent.h"
#include "scope.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"

int which = 0;

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
        } else if (d->type->kind == TYPE_FUNCTION && s->type->kind == TYPE_FUNCTION && !type_param_equals(d->type->params, s->type->params)) {
            ++resolve_errors;
            fprintf(stdout, "resolve error: function %s cannot be redeclared with different parameters.\n", d->name);
        } else {
            symbol_print(d->symbol = s);
        }
    } else {
        symbol_t kind = scope_level() == 1 ? SYMBOL_GLOBAL : SYMBOL_LOCAL;
        d->symbol = symbol_create(kind, d->type, d->name);
        d->symbol->funcdef = 0;
        scope_bind(d->name, d->symbol);
        if (kind == SYMBOL_LOCAL)
            d->symbol->which = which++;
        else
            which = 0;
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

    if (d->type->kind == TYPE_VOID) {
        ++type_errors;
        fprintf(stdout, "type error: standalone variable %s cannot have type void.\n", d->name);
    }

    if (d->value != NULL) {
        struct type *t = expr_typecheck(d->value);
        if (!type_equals(t, d->symbol->type)) {
            ++type_errors;
            fprintf(stdout, "type error: type ");
            type_print(t);
            fprintf(stdout, " (");
            expr_print(d->value, 0);
            fprintf(stdout, ") does not match %s's declaration type ", d->name);
            type_print(d->type);
            fprintf(stdout, ".\n");
        }
        if (d->symbol->kind == SYMBOL_GLOBAL) {
            if (!expr_is_literal(d->value)) {
                ++type_errors;
                fprintf(stdout, "type error: global expression (");
                expr_print(d->value, 0);
                fprintf(stdout, ") cannot be nonliteral.\n");
            }
        }
    }

    if (d->type->kind == TYPE_FUNCTION) {
        if (d->symbol->kind != SYMBOL_GLOBAL) {
            ++type_errors;
            fprintf(stdout, "type error: function %s cannot be declared in a local scope.\n", d->name);
        }
        if (!(type_is_atomic(d->type->subtype) || d->type->subtype->kind == TYPE_VOID)) {
            ++type_errors;
            fprintf(stdout, "type error: function %s cannot return non-atomic, non-void type ", d->name);
            type_print(d->type->subtype);
            fprintf(stdout, ".\n");
        }
        param_list_typecheck(d->type->params);
    }

    /* TODO: array typechecking, will need recursive call */
    if (d->type->kind == TYPE_ARRAY) {
        /* array subtypes must be atomic or array */
        struct type *st = d->type;
        while (st != NULL) {
            if (!(type_is_atomic(st) || st->kind == TYPE_ARRAY)) {
                ++type_errors;
                fprintf(stdout, "type error: array cannot contain non-atomic, non-array type ");
                type_print(st);
                fprintf(stdout, ".\n");
            }
            /* TODO?: array without size? */
            if (st->size != NULL) {
                if (d->symbol->kind == SYMBOL_GLOBAL) {
                    if (st->size->kind != EXPR_INTEGERLIT) {
                        ++type_errors;
                        fprintf(stdout, "type error: global array size expression (");
                        expr_print(st->size, 0);
                        fprintf(stdout, ") must be an integer literal.\n");
                    }
                }
                if (d->symbol->kind == SYMBOL_LOCAL) {
                    if (expr_typecheck(st->size)->kind != TYPE_INTEGER) {
                        ++type_errors;
                        fprintf(stdout, "type error: array size expression (");
                        expr_print(st->size, 0);
                        fprintf(stdout, ") must evaluate to an integer.\n");
                    }
                }
            }
            st = st->subtype;
        }
    }

    if (d->code) {
        if (d->symbol->type->kind != TYPE_FUNCTION) {
            ++type_errors;
            fprintf(stdout, "type error: non-function type ");
            type_print(d->symbol->type);
            fprintf(stdout, " (%s) cannot have a function body.\n", d->symbol->name);
        }
        stmt_typecheck(d->code, d->type->subtype);
    }

    decl_typecheck(d->next);
}
