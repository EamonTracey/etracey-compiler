#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expr.h"
#include "scope.h"

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
            fprintf(stderr, "undeclared identifier!!!\n");
            return;
        }
        e->symbol = s;
    } else {
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
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
