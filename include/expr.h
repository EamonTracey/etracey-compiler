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
    EXPR_ARRLIT,

    EXPR_LIST
} expr_t;

struct expr {
	/* used by all kinds of exprs */
	expr_t kind;
	struct expr *left;
	struct expr *right;

	/* used by various leaf exprs */
	const char *name;
	int literal_value;
    char char_value;
    double float_value;
	const char *string_literal;
	struct symbol *symbol;

    /* track register in which expression is stored */
    int reg;
};

struct expr *expr_create(expr_t kind, struct expr *left, struct expr *right);

struct expr *expr_create_name(const char *n);
struct expr *expr_create_integer_literal(int c);
struct expr *expr_create_float_literal(double c);
struct expr *expr_create_boolean_literal(int c);
struct expr *expr_create_char_literal(char c);
struct expr *expr_create_string_literal(const char *str);

void expr_print(struct expr *e, int paren);
void expr_resolve(struct expr *e);
struct type *expr_typecheck(struct expr *e);

int expr_is_literal(struct expr *e);

int precdif(expr_t kind1, expr_t kind2);

void expr_codegen(struct expr *e);

#endif
