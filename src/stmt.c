#include <stdlib.h>

#include "decl.h"
#include "expr.h"
#include "stmt.h"

struct stmt *stmt_create(stmt_t kind, struct decl *decl, struct expr *init_expr, struct expr *expr, struct expr *next_expr, struct stmt *body, struct stmt *else_body, struct stmt *next) {
    struct stmt *stmt = (struct stmt *)malloc(sizeof(struct stmt));

    if (stmt == NULL)
        return NULL;

    stmt->kind = kind;
    stmt->decl = decl;
    stmt->init_expr = init_expr;
    stmt->expr = expr;
    stmt->next_expr = next_expr;
    stmt->body = body;
    stmt->else_body = else_body;
    stmt->next = next;

    return stmt;
}

void stmt_print(struct stmt *s, int indent) {
}
