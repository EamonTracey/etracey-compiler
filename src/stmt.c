#include <stdlib.h>
#include <stdio.h>

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

//typedef enum {
//    STMT_DECL,
//    STMT_EXPR,
//    STMT_IF_ELSE,
//    STMT_FOR,
//    STMT_PRINT,
//    STMT_RETURN,
//    STMT_BLOCK
//} stmt_t;
void stmt_print(struct stmt *s, int indent) {
    switch (s->kind) {
    case STMT_DECL:
        decl_print(s->decl, indent);
        break;
    case STMT_EXPR:
        expr_print(s->expr);
        fprintf(stdout, ";");
        break;
    case STMT_BLOCK:
        fprintf(stdout, "{\n");
        if (s->body != NULL)
            stmt_print(s->body, indent + 1);
        fprintf(stdout, "}");
        break;
    default:
        break;
    }

    fprintf(stdout, "\n");

    if (s->next != NULL)
        stmt_print(s->next, indent);
}
