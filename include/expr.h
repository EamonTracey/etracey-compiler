#ifndef EXPR_H
#define EXPR_H

#include "symbol.h"

typedef enum {
    EXPR_INC,
    EXPR_DEC,
    EXPR_NOT,
    EXPR_EXP,
    EXPR_MULT,
    EXPR_DIV,
    EXPR_MOD,
    EXPR_PLUS,
    EXPR_MINUS,
    EXPR_LT,
    EXPR_LTE,
    EXPR_GT,
    EXPR_GTE,
    EXPR_EQ,
    EXPR_NOTEQ,
    EXPR_AND,
    EXPR_OR,
    EXPR_ASSIGN,
    EXPR_POS,
    EXPR_NEG,

    EXPR_ARRACC,
    EXPR_FUNCCALL,
    EXPR_IDENT,
    EXPR_INTEGERLIT,
    EXPR_FLOATLIT,
    EXPR_CHARLIT,
    EXPR_STRINGLIT,
    EXPR_BOOLLIT,
    EXPR_ARRAY
} expr_t;

struct expr {
	/* used by all kinds of exprs */
	expr_t kind;
	struct expr *left;
	struct expr *right;

	/* used by various leaf exprs */
	const char *name;
	int literal_value;
	const char *string_literal;
	struct symbol *symbol;
};

struct expr *expr_create(expr_t kind, struct expr *left, struct expr *right);

struct expr *expr_create_name(const char *n);
struct expr *expr_create_integer_literal(int c);
struct expr *expr_create_boolean_literal(int c);
struct expr *expr_create_char_literal(char c);
struct expr *expr_create_string_literal(const char *str);

void expr_print(struct expr *e);

#endif
