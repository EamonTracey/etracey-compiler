#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expr.h"
#include "scope.h"

extern int resolve_errors;
extern int type_errors;

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
        fprintf(stdout, "%s", e->string_literal);
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

    switch (e->kind) {
    case EXPR_INC:
        if (e->left->symbol == NULL) {
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
        if (e->left->symbol == NULL) {
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
        /*TODO: not sure quite honestly.*/
        return NULL;
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
            fprintf(stdout, "cannot perform equality comparison between a ");
            type_print(lt);
            fprintf(stdout, " (");
            expr_print(e->left, 0);
            fprintf(stdout, ") and a ");
            type_print(rt);
            fprintf(stdout, " (");
            expr_print(e->right, 0);
            fprintf(stdout, ").\n");
        }
        /* TODO non-atomic types cannot be compared */
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
        if (e->left->symbol == NULL) {
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
        /* TODO: nonatomics? */
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
    default:
        return NULL;
    }
}

/*
typedef enum {
    EXPR_ARRLIT,

    EXPR_LIST
} expr_t;
*/

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
