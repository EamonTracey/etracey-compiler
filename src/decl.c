#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expr.h"
#include "indent.h"
#include "scratch.h"
#include "scope.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"

int which = 0;

extern int resolve_errors;
extern int type_errors;

struct symbol *codegen_func_symbol;

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
        s->n_locals = which;
        scope_exit();
        scope_exit();
        struct param_list *p = d->type->params;
        s->n_params = 0;
        while (p != NULL) {
            ++s->n_params;
            p = p->next;
        }
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
            } else if (st->kind == TYPE_ARRAY ) {
                ++type_errors;
                fprintf(stdout, "type error: array must have explicit size.\n");
            }
            st = st->subtype;
        }
        if (d->symbol->kind == SYMBOL_LOCAL && d->value != NULL) {
            ++type_errors;
            fprintf(stdout, "type error: cannot initialize array %s in a local scope.\n", d->name);
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

void decl_codegen(struct decl *d) {
    if (d == NULL)
        return;

    /* Global variable declaration. */
    /* TODO: global variables without initialization */
    if (d->symbol->kind == SYMBOL_GLOBAL && d->type->kind != TYPE_FUNCTION) {
        if (d->value->kind == EXPR_INTEGERLIT || d->value->kind == EXPR_BOOLLIT)
            fprintf(stdout, "%s: .quad %d\n", d->symbol->name, d->value->literal_value);
        else if (d->value->kind == EXPR_CHARLIT)
            fprintf(stdout, "%s: .quad %d\n", d->symbol->name, d->value->char_value);
        else if (d->value->kind == EXPR_STRINGLIT) {
            fprintf(stdout, "%s: .string ", d->symbol->name);
            expr_print(d->value, 0);
            fprintf(stdout, "\n");
        } else if (d->value->kind == EXPR_ARRLIT) {
            if (d->type->subtype->kind != TYPE_INTEGER) {
                fprintf(stdout, "codegen error: missing support for non-integer arrays.\n");
                exit(1);
            }
            fprintf(stdout, "%s: .quad ", d->symbol->name);
            expr_print(d->value->left, 0);
            fprintf(stdout, "\n");
        }
        else {
            fprintf(stdout, "codegen error: missing support for floats.\n");
            exit(1);
        }
    }

    /* Local variable declaration with initialization. */
    else if (d->symbol->kind == SYMBOL_LOCAL && d->value != NULL) {
        expr_codegen(d->value);
        fprintf(stdout, "MOVQ %s, %s\n", scratch_name(d->value->reg), symbol_codegen(d->symbol));
        scratch_free(d->value->reg);
    }

    /* Function definition. */
    else if (d->code != NULL) {
        if (d->symbol->n_params > 6) {
            fprintf(stdout, "codegen error: missing support for functions with greater than 6 parameters.\n");
            exit(1);
        }
        /*
         * Functions must do multiple things:
         * 1. Save and update the base pointer.
         * 2. Save arguments onto the stack.
         * 3. Allocate space for local variables on stack.
         * 4. Save callee-saved registers.
         * 5. Function body.
         * 6. Restore callee-saved registers.
         * 7. Reset stack.
         * 8. Return.
         */
        /* Write function label. */
        fprintf(stdout, ".global %s\n", d->name);
        fprintf(stdout, "%s:\n", d->name);
        /* 1. Save and update the base pointer. */
        fprintf(stdout, "PUSHQ %%rbp\n");
        fprintf(stdout, "MOVQ %%rsp, %%rbp\n");
        /* 2. Save arguments onto stack. */
        static const char *arg_regs[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };
        for (int i = 0; i < d->symbol->n_params; ++i)
            fprintf(stdout, "PUSHQ %s\n", arg_regs[i]);
        /* 3. Allocate space for local variables on stack. */
        fprintf(stdout, "SUBQ $%d, %%rsp\n", 8 * d->symbol->n_locals);
        /* 4. Save callee-saved registers. */
        fprintf(stdout, "PUSHQ %%rbx\n");
        fprintf(stdout, "PUSHQ %%r12\n");
        fprintf(stdout, "PUSHQ %%r13\n");
        fprintf(stdout, "PUSHQ %%r14\n");
        fprintf(stdout, "PUSHQ %%r15\n");
        int rbx_before = scratch_check(0); scratch_free(0);
        int r12_before = scratch_check(3); scratch_free(3);
        int r13_before = scratch_check(4); scratch_free(4);
        int r14_before = scratch_check(5); scratch_free(5);
        int r15_before = scratch_check(6); scratch_free(6);
        /* Code generation within function must know about the function. */
        codegen_func_symbol = d->symbol;
        /* 5. Function body. */
        stmt_codegen(d->code);
        /* Label the function epilogue to support return statements. */
        fprintf(stdout, ".%s_epilogue:\n", d->name);
        /* 6. Restore callee-saved registers. */
        fprintf(stdout, "POPQ %%r15\n");
        fprintf(stdout, "POPQ %%r14\n");
        fprintf(stdout, "POPQ %%r13\n");
        fprintf(stdout, "POPQ %%r12\n");
        fprintf(stdout, "POPQ %%rbx\n");
        scratch_set(0, rbx_before);
        scratch_set(3, r12_before);
        scratch_set(4, r13_before);
        scratch_set(5, r14_before);
        scratch_set(6, r15_before);
        /* 7. Reset stack. */
        fprintf(stdout, "MOVQ %%rbp, %%rsp\n");
        fprintf(stdout, "POPQ %%rbp\n");
        /* 8. Return */
        fprintf(stdout, "RET\n");
    }

    decl_codegen(d->next);
}
