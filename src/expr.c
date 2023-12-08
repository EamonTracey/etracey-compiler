#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codegen.h"
#include "encode.h"
#include "expr.h"
#include "label.h"
#include "scratch.h"
#include "scope.h"

extern int resolve_errors;
extern int type_errors;

extern FILE *codegen_out;

struct expr *expr_create(expr_t kind, struct expr *left, struct expr *right) {
    struct expr *expr = (struct expr *)malloc(sizeof(struct expr));
    
    if (expr == NULL)
        return NULL;

    expr->kind = kind;
    expr->left = left;
    expr->right = right;

    expr->name = NULL;
    expr->symbol = NULL;
    expr->literal_value = 0;
    expr->string_literal = NULL;

    return expr;
}

struct expr *expr_create_name(const char *n) {
    struct expr *expr = (struct expr *)malloc(sizeof(struct expr));

    expr->kind = EXPR_IDENT;
    expr->name = strdup(n);

    return expr;
}

struct expr *expr_create_integer_literal(int c) {
    struct expr *expr = (struct expr *)malloc(sizeof(struct expr));

    expr->kind = EXPR_INTEGERLIT;
    expr->literal_value = c;

    return expr;
}

struct expr *expr_create_float_literal(double c) {
    struct expr *expr = (struct expr *)malloc(sizeof(struct expr));

    expr->kind = EXPR_FLOATLIT;
    expr->float_value = c;

    return expr;
}

struct expr *expr_create_boolean_literal(int c) {
    struct expr *expr = (struct expr *)malloc(sizeof(struct expr));

    expr->kind = EXPR_BOOLLIT;
    expr->literal_value = c;

    return expr;
}

struct expr *expr_create_char_literal(char c) {
    struct expr *expr = (struct expr *)malloc(sizeof(struct expr));

    expr->kind = EXPR_CHARLIT;
    expr->char_value = c;

    return expr;
}

struct expr *expr_create_string_literal(const char *str) {
    struct expr *expr = (struct expr *)malloc(sizeof(struct expr));

    expr->kind = EXPR_STRINGLIT;
    expr->string_literal = str;

    return expr;
}

void expr_print(struct expr *e, int paren) {
    if (e == NULL)
        return;

    char s[2048];

    /* 
     * Parentheticals are necssary if the parent operator
     * has higher precedence than the child operator.
     * Note: Parentheticals also necessary for some same-precedence,
     * non-associative operators (e.g., 1 - (2 + 3)).
     */
    if (paren)
        fprintf(stdout, "(");

    switch (e->kind) {
    case EXPR_INC:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "++");
        break;
    case EXPR_DEC:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "--");
        break;
    case EXPR_NOT:
        fprintf(stdout, "!");
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0 || e->left->kind == EXPR_NOT);
        break;
    case EXPR_EXP:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "^");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_MULT:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "*");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_DIV:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "/");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0 || e->right->kind == EXPR_DIV);
        break;
    case EXPR_MOD:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "%%");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_PLUS:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "+");
        if (e->right->kind == EXPR_POS) fprintf(stdout, " ");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_MINUS:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "-");
        if (e->right->kind == EXPR_NEG) fprintf(stdout, " ");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0 || e->right->kind == EXPR_PLUS);
        break;
    case EXPR_LT:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "<");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_LTE:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "<=");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_GT:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, ">");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_GTE:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, ">=");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_EQ:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "==");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_NOTEQ:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "!=");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_AND:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "&&");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_OR:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "||");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_ASSIGN:
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        fprintf(stdout, "=");
        expr_print(e->right, precdif(e->kind, e->right->kind) > 0);
        break;
    case EXPR_POS:
        fprintf(stdout, "+");
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        break;
    case EXPR_NEG:
        fprintf(stdout, "-");
        expr_print(e->left, precdif(e->kind, e->left->kind) > 0);
        break;
    case EXPR_ARRACC:
        expr_print(e->left, 0);
        fprintf(stdout, "[");
        expr_print(e->right, 0);
        fprintf(stdout, "]");
        break;
    case EXPR_FUNCCALL:
        expr_print(e->left, 0);
        fprintf(stdout, "(");
        expr_print(e->right, 0);
        fprintf(stdout, ")");
        break;
    case EXPR_IDENT:
        fprintf(stdout, "%s", e->name);
        break;
    case EXPR_INTEGERLIT:
        fprintf(stdout, "%d", e->literal_value);
        break;
    case EXPR_FLOATLIT:
        fprintf(stdout, "%lf", e->float_value);
        break;
    case EXPR_CHARLIT:
        fprintf(stdout, "'\\0x%02x'", e->char_value);
        break;
    case EXPR_STRINGLIT:
        string_encode(e->string_literal, s);
        fprintf(stdout, "%s", s);
        break;
    case EXPR_BOOLLIT:
        fprintf(stdout, e->literal_value ? "true" : "false");
        break;
    case EXPR_ARRLIT:
        fprintf(stdout, "{");
        expr_print(e->left, 0);
        fprintf(stdout, "}");
        break;
    case EXPR_LIST:
        expr_print(e->left, 0);
        if (e->right != NULL) {
            fprintf(stdout, ",");
            expr_print(e->right, 0);
        }
    }

    if (paren > 0)
        fprintf(stdout, ")");
}

void expr_resolve(struct expr *e) {
    if (e == NULL)
        return;

    if (e->kind == EXPR_IDENT) {
        struct symbol *s = scope_lookup(e->name);
        if (s == NULL) {
            ++resolve_errors;
            fprintf(stdout, "resolve error: %s is not defined.\n", e->name);
            return;
        }
        e->symbol = s;
        symbol_print(e->symbol);
    } else {
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
}

struct type *expr_typecheck(struct expr *e) {
    if (e == NULL)
        return NULL;

    struct type *lt = expr_typecheck(e->left);
    struct type *rt = expr_typecheck(e->right);

    struct expr *q;
    struct expr *si;

    int count = 0;

    switch (e->kind) {
    case EXPR_INC:
        if (e->left->symbol == NULL && e->left->kind != EXPR_ARRACC) {
            ++type_errors;
            fprintf(stdout, "type error: cannot increment a non-variable.\n");
        }
        if (lt->kind != TYPE_INTEGER) {
            ++type_errors;
            fprintf(stdout, "type error: cannot increment a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_DEC:
        if (e->left->symbol == NULL && e->left->kind != EXPR_ARRACC) {
            ++type_errors;
            fprintf(stdout, "type error: cannot decrement a non-variable.\n");
        }
        if (lt->kind != TYPE_INTEGER) {
            ++type_errors;
            fprintf(stdout, "type error: cannot decrement a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_NOT:
        if (lt->kind != TYPE_BOOLEAN) {
            ++type_errors;
            fprintf(stdout, "type error: cannot apply logical not to a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_BOOLEAN, NULL, NULL, NULL);
    case EXPR_EXP:
        if (!((lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) || (lt->kind == TYPE_FLOAT && rt->kind == TYPE_FLOAT))) {
            ++type_errors;
            fprintf(stdout, "type error: cannot exponentiate a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") with a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(lt->kind == TYPE_FLOAT ? TYPE_FLOAT : TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_MULT:
        if (!((lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) || (lt->kind == TYPE_FLOAT && rt->kind == TYPE_FLOAT))) {
            ++type_errors;
            fprintf(stdout, "type error: cannot multiply a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") with a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(lt->kind == TYPE_FLOAT ? TYPE_FLOAT : TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_DIV:
        if (!((lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) || (lt->kind == TYPE_FLOAT && rt->kind == TYPE_FLOAT))) {
            ++type_errors;
            fprintf(stdout, "type error: cannot divide a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") by a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(lt->kind == TYPE_FLOAT ? TYPE_FLOAT : TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_MOD:
        if (!(lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER)) {
            ++type_errors;
            fprintf(stdout, "type error: cannot take the modulus of a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") with a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_PLUS:
        if (!((lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) || (lt->kind == TYPE_FLOAT && rt->kind == TYPE_FLOAT))) {
            ++type_errors;
            fprintf(stdout, "type error: cannot add a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") with a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(lt->kind == TYPE_FLOAT ? TYPE_FLOAT : TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_MINUS:
        if (!((lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) || (lt->kind == TYPE_FLOAT && rt->kind == TYPE_FLOAT))) {
            ++type_errors;
            fprintf(stdout, "type error: cannot subtract a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") by a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(lt->kind == TYPE_FLOAT ? TYPE_FLOAT : TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_LT:
        if (!((lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) || (lt->kind == TYPE_FLOAT && rt->kind == TYPE_FLOAT))) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform less-than comparison between a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") and a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_BOOLEAN, NULL, NULL, NULL);
    case EXPR_LTE:
        if (!((lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) || (lt->kind == TYPE_FLOAT && rt->kind == TYPE_FLOAT))) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform less-than-equal comparison between a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") and a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_BOOLEAN, NULL, NULL, NULL);
    case EXPR_GT:
        if (!((lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) || (lt->kind == TYPE_FLOAT && rt->kind == TYPE_FLOAT))) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform greater-than comparison between a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") and a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_BOOLEAN, NULL, NULL, NULL);
    case EXPR_GTE:
        if (!((lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) || (lt->kind == TYPE_FLOAT && rt->kind == TYPE_FLOAT))) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform greater-than-equal comparison between a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") and a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_BOOLEAN, NULL, NULL, NULL);
    case EXPR_EQ:
    case EXPR_NOTEQ:
        if (!type_equals(lt, rt)) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform equality comparison between a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") and a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        if (!type_is_atomic(lt)) {
            ++type_errors;
            fprintf(stdout, "type error: non-atomic type ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") cannot be compared.\n");
        }
        if (!type_is_atomic(rt)) {
            ++type_errors;
            fprintf(stdout, "type error: non-atomic type ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ") cannot be compared.\n");
        }
        return type_create(TYPE_BOOLEAN, NULL, NULL, NULL);
    case EXPR_AND:
        if (!(lt->kind == TYPE_BOOLEAN && rt->kind == TYPE_BOOLEAN)) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform logical and between a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") and a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_BOOLEAN, NULL, NULL, NULL);
    case EXPR_OR:
        if (!(lt->kind == TYPE_BOOLEAN && rt->kind == TYPE_BOOLEAN)) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform logical or between a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") and a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(TYPE_BOOLEAN, NULL, NULL, NULL);
    case EXPR_ASSIGN:
        if (e->left->symbol == NULL && e->left->kind != EXPR_ARRACC) {
            ++type_errors;
            fprintf(stdout, "type error: cannot assign to a non-variable.\n");
        }
        if (!type_equals(lt, rt)) {
            ++type_errors;
            fprintf(stdout, "type error: cannot assign non-matching type ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ") to a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ").\n");
        }
        if (!(type_is_atomic(lt) || lt->kind == TYPE_ARRAY)) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform assignment with non-atomic, non-array type ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ").\n");
        }
        if (!(type_is_atomic(rt) || rt->kind == TYPE_ARRAY)) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform assignment with non-atomic, non-array type ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(lt->kind, NULL, NULL, NULL);
    case EXPR_POS:
    case EXPR_NEG:
        if (lt->kind != TYPE_INTEGER && lt->kind != TYPE_FLOAT) {
            ++type_errors;
            fprintf(stdout, "type error: cannot apply unary operator to a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ").\n");
        }
        return type_create(lt->kind, NULL, NULL, NULL);
    case EXPR_IDENT:
        return type_create(e->symbol->type->kind, e->symbol->type->subtype, e->symbol->type->params, e->symbol->type->size);
    case EXPR_INTEGERLIT:
        return type_create(TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_FLOATLIT:
        return type_create(TYPE_FLOAT, NULL, NULL, NULL);
    case EXPR_CHARLIT:
        return type_create(TYPE_CHARACTER, NULL, NULL, NULL);
    case EXPR_STRINGLIT:
        return type_create(TYPE_STRING, NULL, NULL, NULL);
    case EXPR_BOOLLIT:
        return type_create(TYPE_BOOLEAN, NULL, NULL, NULL);
    case EXPR_ARRACC:
        if (lt->kind != TYPE_ARRAY) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform array access on non-array type ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ").\n");
        }
        if (rt->kind != TYPE_INTEGER) {
            ++type_errors;
            fprintf(stdout, "type error: cannot index array with type ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        return lt->subtype != NULL ? lt->subtype : type_create(TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_FUNCCALL:
        /* TODO: fix double error issue when argument has type error */
        if (lt->kind != TYPE_FUNCTION) {
            ++type_errors;
            fprintf(stdout, "type error: cannot perform function call on non-function type ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ").\n");
        } else {
            struct param_list *par = lt->params;
            struct expr *arg = e->right;
            while (par != NULL && arg != NULL) {
                struct type *argtype = expr_typecheck(arg->left);
                if (!type_equals(par->type, argtype)) {
                    ++type_errors;
                    fprintf(stdout, "type error: argument of type ");
                    type_print(argtype);
                    fprintf(stdout, " (");
                    expr_print(arg->left, 0);
                    fprintf(stdout, ") does not match function %s's parameter %s's type ", e->left->name, par->name);
                    type_print(par->type);
                    fprintf(stdout, ".\n");
                }
                par = par->next;
                arg = arg->right;
            }
            if (par == NULL && arg != NULL) {
                ++type_errors;
                int count = 0;
                while (arg != NULL) {
                    ++count;
                    expr_typecheck(arg);
                    arg = arg->right;
                }
                fprintf(stdout, "type error: function %s received %d extra arguments.\n", e->left->name, count);
            }
            if (par != NULL && arg == NULL) {
                ++type_errors;
                int count = 0;
                while (par != NULL) {
                    ++count;
                    par = par->next;
                }
                fprintf(stdout, "type error: function %s requires %d more arguments.\n", e->left->name, count);
            }
        }
        return lt->subtype != NULL ? lt->subtype : type_create(TYPE_INTEGER, NULL, NULL, NULL);
    case EXPR_ARRLIT:
        q = e->left;
        lt = expr_typecheck(q->left);
        count = 0;
        while (q != NULL) {
            ++count;
            rt = expr_typecheck(q->left);
            if (!(type_is_atomic(rt) || rt->kind == TYPE_ARRAY)) {
                ++type_errors;
                fprintf(stdout, "type error: array literal cannot have element with type ");
                type_print(rt);
                fprintf(stdout, " (");
                expr_print(q->left, 0);
                fprintf(stdout, ").\n");
            }
            if (!type_equals(lt, rt)) {
                ++type_errors;
                fprintf(stdout, "type error: array literal cannot have mismatching types ");
                type_print(lt);
                fprintf(stdout, " and ");
                type_print(rt);
                fprintf(stdout, "\n");
            }
            q = q->right;
        }
        si = expr_create(EXPR_INTEGERLIT, NULL, NULL);
        si->literal_value = count;
        return type_create(TYPE_ARRAY, lt, NULL, si);
    default:
        return NULL;
    }
}

int expr_is_literal(struct expr *e) {
    if (e->kind == EXPR_ARRLIT) {
        e = e->left;
        while (e != NULL) {
            if (!expr_is_literal(e->left))
                return 0;
            e = e->right;
        }
        return 1;
    }

    return e->kind == EXPR_INTEGERLIT
        || e->kind == EXPR_FLOATLIT
        || e->kind == EXPR_CHARLIT
        || e->kind == EXPR_STRINGLIT
        || e->kind == EXPR_BOOLLIT
        || (e->kind == EXPR_NEG && e->left->kind == EXPR_INTEGERLIT)
        || (e->kind == EXPR_POS && e->left->kind == EXPR_INTEGERLIT);
}

int precedences[] = {
    8, 8, 7, 6, 5, 5, 5, 4, 4,
    3, 3, 3, 3, 3, 3, 2, 1, 0,
    7, 7, 9, 9, 9, 9, 9, 9, 9,
    9, 9, -1
};

/*
 * These tables shall remain a WIP for now.
int left_assoc[] = {
    0, 0, 0, 0, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1
};

int right_assoc[] = {
    0, 0, 0, 1, 1, 0, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1
};
*/

int precdif(expr_t kind1, expr_t kind2) {
    return precedences[kind1] - precedences[kind2];
}

void expr_codegen(struct expr *e) {
    int reg;
    int label;
    int true_label;
    int done_label;

    struct expr *elist;

    int arg = 0;
    static char *arg_regs[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };
    int float_arg = 0;
    static char *float_arg_regs[] = { "%xmm0", "%xmm1", "%xmm2" };
    char s[2048];

    switch (e->kind) {
    case EXPR_INC:
        expr_codegen(e->left);
        fprintf(codegen_out, "    incq %s\n", scratch_name(e->left->reg));
        if (e->left->kind == EXPR_IDENT)
            fprintf(codegen_out, "    movq %s, %s\n", scratch_name(e->left->reg), symbol_codegen(e->left->symbol));
        else /* array access */ {
            expr_codegen(e->left->left);
            expr_codegen(e->left->right);
            fprintf(codegen_out, "    movq %s, 0(%s, %s, 8)\n", scratch_name(e->left->reg), scratch_name(e->left->left->reg), scratch_name(e->left->right->reg));
            scratch_free(e->left->left->reg);
            scratch_free(e->left->right->reg);
        }
        fprintf(codegen_out, "    decq %s\n", scratch_name(e->left->reg));
        e->reg = e->left->reg;
        break;
    case EXPR_DEC:
        expr_codegen(e->left);
        fprintf(codegen_out, "    decq %s\n", scratch_name(e->left->reg));
        if (e->left->kind == EXPR_IDENT)
            fprintf(codegen_out, "    movq %s, %s\n", scratch_name(e->left->reg), symbol_codegen(e->left->symbol));
        else /* array access */ {
            expr_codegen(e->left->left);
            expr_codegen(e->left->right);
            fprintf(codegen_out, "    movq %s, 0(%s, %s, 8)\n", scratch_name(e->left->reg), scratch_name(e->left->left->reg), scratch_name(e->left->right->reg));
            scratch_free(e->left->left->reg);
            scratch_free(e->left->right->reg);
        }
        fprintf(codegen_out, "    incq %s\n", scratch_name(e->left->reg));
        e->reg = e->left->reg;
        break;
    case EXPR_EXP:
        expr_codegen(e->left);
        expr_codegen(e->right);
        fprintf(codegen_out, "    movq %s, %%rdi\n", scratch_name(e->left->reg));
        fprintf(codegen_out, "    movq %s, %%rsi\n", scratch_name(e->right->reg));
        codegen_funccall("integer_power");
        fprintf(codegen_out, "    movq %%rax, %s\n", scratch_name(e->right->reg));
        scratch_free(e->left->reg);
        e->reg = e->right->reg;
        break;
    case EXPR_MULT:
        expr_codegen(e->left);
        expr_codegen(e->right);
        if (expr_typecheck(e->left)->kind == TYPE_FLOAT) {
            fprintf(codegen_out, "    mulsd %s, %s\n", scratch_float_name(e->left->reg), scratch_float_name(e->right->reg));
            scratch_float_free(e->left->reg);
        } else{
            fprintf(codegen_out, "    movq %s, %%rax\n", scratch_name(e->right->reg));
            fprintf(codegen_out, "    imulq %s\n", scratch_name(e->left->reg));
            fprintf(codegen_out, "    movq %%rax, %s\n", scratch_name(e->right->reg));
            scratch_free(e->left->reg);
        }
        e->reg = e->right->reg;
        break;
    case EXPR_DIV:
        expr_codegen(e->left);
        expr_codegen(e->right);
        if (expr_typecheck(e->left)->kind == TYPE_FLOAT) {
            fprintf(codegen_out, "    divsd %s, %s\n", scratch_float_name(e->right->reg), scratch_float_name(e->left->reg));
            scratch_float_free(e->right->reg);
        e->reg = e->left->reg;
        } else{
            fprintf(codegen_out, "    movq %s, %%rax\n", scratch_name(e->left->reg));
            fprintf(codegen_out, "    cqo\n");
            fprintf(codegen_out, "    idivq %s\n", scratch_name(e->right->reg));
            fprintf(codegen_out, "    movq %%rax, %s\n", scratch_name(e->right->reg));
            scratch_free(e->left->reg);
        e->reg = e->right->reg;
        }
        break;
    case EXPR_MOD:
        expr_codegen(e->left);
        expr_codegen(e->right);
        fprintf(codegen_out, "    movq %s, %%rax\n", scratch_name(e->left->reg));
        fprintf(codegen_out, "    cqo\n");
        fprintf(codegen_out, "    idivq %s\n", scratch_name(e->right->reg));
        fprintf(codegen_out, "    movq %%rdx, %s\n", scratch_name(e->right->reg));
        scratch_free(e->left->reg);
        e->reg = e->right->reg;
        break;
    case EXPR_PLUS:
        expr_codegen(e->left);
        expr_codegen(e->right);
        if (expr_typecheck(e->left)->kind == TYPE_FLOAT) {
            fprintf(codegen_out, "    addsd %s, %s\n", scratch_float_name(e->left->reg), scratch_float_name(e->right->reg));
            scratch_float_free(e->left->reg);
        } else {
            fprintf(codegen_out, "    addq %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
            scratch_free(e->left->reg);
        }
        e->reg = e->right->reg;
        break;
    case EXPR_MINUS:
        expr_codegen(e->left);
        expr_codegen(e->right);
        if (expr_typecheck(e->left)->kind == TYPE_FLOAT) {
            fprintf(codegen_out, "    subsd %s, %s\n", scratch_float_name(e->right->reg), scratch_float_name(e->left->reg));
            scratch_float_free(e->right->reg);
        } else {
            fprintf(codegen_out, "    subq %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg));
            scratch_free(e->right->reg);
        }
        e->reg = e->left->reg;
        break;
    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_GT:
    case EXPR_GTE:
    case EXPR_EQ:
    case EXPR_NOTEQ:
        true_label = label_create();
        done_label = label_create();
        expr_codegen(e->left);
        expr_codegen(e->right);
        if (expr_typecheck(e->left)->kind == TYPE_FLOAT) {
            if (e->kind == EXPR_LT)
                fprintf(codegen_out, "    cmpltsd %s, %s\n", scratch_float_name(e->left->reg), scratch_float_name(e->right->reg));
            else if (e->kind == EXPR_LTE)
                fprintf(codegen_out, "    cmplesd %s, %s\n", scratch_float_name(e->left->reg), scratch_float_name(e->right->reg));
            else if (e->kind == EXPR_GT)
                fprintf(codegen_out, "    cmpnlesd %s, %s\n", scratch_float_name(e->left->reg), scratch_float_name(e->right->reg));
            else if (e->kind == EXPR_GTE)
                fprintf(codegen_out, "    cmpnltsd %s, %s\n", scratch_float_name(e->left->reg), scratch_float_name(e->right->reg));
            else if (e->kind == EXPR_EQ)
                fprintf(codegen_out, "    cmpeqsd %s, %s\n", scratch_float_name(e->left->reg), scratch_float_name(e->right->reg));
            else if (e->kind == EXPR_NOTEQ)
                fprintf(codegen_out, "    cmpneqsd %s, %s\n", scratch_float_name(e->left->reg), scratch_float_name(e->right->reg));
            e->reg = e->right->reg;
            scratch_float_free(e->left->reg);
        } else{
        fprintf(codegen_out, "    cmpq %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg));
        if (e->kind == EXPR_LT)
            fprintf(codegen_out, "    jl %s\n", label_name(true_label));
        else if (e->kind == EXPR_LTE)
            fprintf(codegen_out, "    jle %s\n", label_name(true_label));
        else if (e->kind == EXPR_GT)
            fprintf(codegen_out, "    jg %s\n", label_name(true_label));
        else if (e->kind == EXPR_GTE)
            fprintf(codegen_out, "    jge %s\n", label_name(true_label));
        else if (e->kind == EXPR_EQ)
            fprintf(codegen_out, "    je %s\n", label_name(true_label));
        else if (e->kind == EXPR_NOTEQ)
            fprintf(codegen_out, "    jne %s\n", label_name(true_label));
        fprintf(codegen_out, "    movq $0, %s\n", scratch_name(e->right->reg));
        fprintf(codegen_out, "    jmp %s\n", label_name(done_label));
        fprintf(codegen_out, "%s:\n", label_name(true_label));
        fprintf(codegen_out, "    movq $-1, %s\n", scratch_name(e->right->reg));
        fprintf(codegen_out, "%s:\n", label_name(done_label));
        scratch_free(e->left->reg);
        e->reg = e->right->reg;
        }
        break;
    case EXPR_AND:
    case EXPR_OR:
        expr_codegen(e->left);
        expr_codegen(e->right);
        if (e->kind == EXPR_AND)
            fprintf(codegen_out, "    andq %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
        else
            fprintf(codegen_out, "    orq %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
        scratch_free(e->left->reg);
        e->reg = e->right->reg;
        break;
    case EXPR_NOT:
        expr_codegen(e->left);
        fprintf(codegen_out, "    notq %s\n", scratch_name(e->left->reg));
        e->reg = e->left->reg;
        break;
    case EXPR_ASSIGN:
        /* TODO: string and array assignment */
        expr_codegen(e->right);
        if (e->left->kind == EXPR_ARRACC) {
            expr_codegen(e->left->left);
            expr_codegen(e->left->right);
            fprintf(codegen_out, "    movq %s, 0(%s, %s, 8)\n", scratch_name(e->right->reg), scratch_name(e->left->left->reg), scratch_name(e->left->right->reg));
            scratch_free(e->left->left->reg);
            scratch_free(e->left->right->reg);
        } else if (expr_typecheck(e->left)->kind == TYPE_FLOAT) {
            fprintf(codegen_out, "    movsd %s, %s\n", scratch_float_name(e->right->reg), symbol_codegen(e->left->symbol));
        } else
            fprintf(codegen_out, "    movq %s, %s\n", scratch_name(e->right->reg), symbol_codegen(e->left->symbol));
        e->reg = e->right->reg;
        break;
    case EXPR_POS:
        expr_codegen(e->left);
        e->reg = e->left->reg;
        break;
    case EXPR_NEG:
        expr_codegen(e->left);
        if (e->left->kind == EXPR_INTEGERLIT)
            fprintf(codegen_out, "    movq $-%d, %s\n", e->left->literal_value, scratch_name(e->left->reg));
        else {
            reg = scratch_alloc();
            fprintf(codegen_out, "    movq $-1, %s\n", scratch_name(reg));
            fprintf(codegen_out, "    movq %s, %%rax\n", scratch_name(e->left->reg));
            fprintf(codegen_out, "    imulq %s\n", scratch_name(reg));
            fprintf(codegen_out, "    movq %%rax, %s\n", scratch_name(e->left->reg));
            scratch_free(reg);
        }
        e->reg = e->left->reg;
        break;
    case EXPR_ARRACC:
        expr_codegen(e->left);
        expr_codegen(e->right);
        if (expr_typecheck(e->left)->subtype->kind == TYPE_FLOAT){
            fprintf(codegen_out, "    movsd 0(%s, %s, 8), %s\n", scratch_float_name(e->left->reg), scratch_float_name(e->right->reg), scratch_float_name(e->right->reg));
        scratch_float_free(e->left->reg);
        }else{
            fprintf(codegen_out, "    movq 0(%s, %s, 8), %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg), scratch_name(e->right->reg));
        scratch_free(e->left->reg);
        }
        e->reg = e->right->reg;
        break;
    case EXPR_FUNCCALL:
        if (e->left->symbol->n_params > 6) {
            fprintf(stdout, "codegen error: missing support for function calls with greater than 6 arguments.\n");
            exit(1);
        }
        /* evaluate the arguments. */
        elist = e->right;
        while (elist != NULL) {
            expr_codegen(elist->left);
            elist = elist->right;
        }
        /* pass arguments into registers. */
        elist = e->right;
        int nfloats = 0;
        while (elist != NULL) {
            if (expr_typecheck(elist->left)->kind == TYPE_FLOAT){
                ++nfloats;
                fprintf(codegen_out, "    movsd %s, %s\n", scratch_float_name(elist->left->reg), float_arg_regs[float_arg++]);
                scratch_float_free(elist->left->reg);
            }else{
                fprintf(codegen_out, "    movq %s, %s\n", scratch_name(elist->left->reg), arg_regs[arg++]);
                scratch_free(elist->left->reg);
            }
            elist = elist->right;
        }
        /* helper :) */
        fprintf(codegen_out, "    movq $%d, %%rax\n", nfloats);
        codegen_funccall(e->left->symbol->name);
        if (e->left->symbol->type->subtype->kind == TYPE_FLOAT){
            reg = scratch_float_alloc();
            fprintf(codegen_out, "    movsd %%xmm0, %s\n", scratch_float_name(reg));
        }else{
            reg = scratch_alloc();
            fprintf(codegen_out, "    movq %%rax, %s\n", scratch_name(reg));
        }
        e->reg = reg;
        break;
    case EXPR_IDENT:
        if (e->symbol->type->kind == TYPE_ARRAY || (e->symbol->type->kind == TYPE_STRING && e->symbol->kind == SYMBOL_GLOBAL)) {
        reg = scratch_alloc();
            fprintf(codegen_out, "    leaq %s, %s\n", symbol_codegen(e->symbol), scratch_name(reg));
        } else if (e->symbol->type->kind == TYPE_FLOAT) {
            reg = scratch_float_alloc();
            fprintf(codegen_out, "    movsd %s, %s\n", symbol_codegen(e->symbol), scratch_float_name(reg));
        }
        else{
        reg = scratch_alloc();
            fprintf(codegen_out, "    movq %s, %s\n", symbol_codegen(e->symbol), scratch_name(reg));
        }
        e->reg = reg;
        break;
    case EXPR_INTEGERLIT:
        reg = scratch_alloc();
        fprintf(codegen_out, "    movq $%d, %s\n", e->literal_value, scratch_name(reg));
        e->reg = reg;
        break;
    case EXPR_BOOLLIT:
        reg = scratch_alloc();
        fprintf(codegen_out, "    movq $%d, %s\n", e->literal_value, scratch_name(reg));
        e->reg = reg;
        break;
    case EXPR_CHARLIT:
        reg = scratch_alloc();
        fprintf(codegen_out, "    movq $%d, %s\n", e->char_value, scratch_name(reg));
        e->reg = reg;
        break;
    case EXPR_STRINGLIT:
        reg = scratch_alloc();
        label = label_create();
        string_encode(e->string_literal, s);
        fprintf(codegen_out, ".data\n");
        fprintf(codegen_out, "%s: .string %s\n", label_name(label), s);
        fprintf(codegen_out, ".text\n");
        fprintf(codegen_out, "    leaq %s, %s\n", label_name(label), scratch_name(reg));
        e->reg = reg;
        break;
    case EXPR_FLOATLIT:
        reg = scratch_float_alloc();
        label = label_create();
        fprintf(codegen_out, ".data\n");
        fprintf(codegen_out, "%s: .double %lf\n", label_name(label), e->float_value);
        fprintf(codegen_out, ".text\n");
        fprintf(codegen_out, "    movsd %s, %s\n", label_name(label), scratch_float_name(reg));
        e->reg = reg;
        break;
    case EXPR_ARRLIT:
        fprintf(stdout, "codegen error: missing support for dynamic arrays.\n");
        exit(1);
    default:
        fprintf(stdout, "codegen error: missing support.\n");
        exit(1);
    }
}
