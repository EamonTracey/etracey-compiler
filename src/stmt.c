#include <stdlib.h>
#include <stdio.h>

#include "decl.h"
#include "expr.h"
#include "indent.h"
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
    if (s == NULL)
        return;

    switch (s->kind) {
    case STMT_DECL:
        decl_print(s->decl, indent);
        break;
    case STMT_EXPR:
        indent_print(indent);
        expr_print(s->expr, 0);
        fprintf(stdout, ";");
        fprintf(stdout, "\n");
        break;
    case STMT_BLOCK:
        fprintf(stdout, "{\n");
        if (s->body != NULL)
            stmt_print(s->body, indent);
        indent_print(indent - 1);
        fprintf(stdout, "}");
        fprintf(stdout, "\n");
        break;
    case STMT_IF_ELSE:
        indent_print(indent);
        fprintf(stdout, "if (");
        expr_print(s->expr, 0);
        fprintf(stdout, ") ");
        if (s->body->kind != STMT_BLOCK)
            fprintf(stdout, "\n");
        stmt_print(s->body, indent + 1);
        if (s->else_body != NULL) {
            indent_print(indent);
            fprintf(stdout, "else ");
            if (s->else_body->kind != STMT_BLOCK)
                fprintf(stdout, "\n");
            stmt_print(s->else_body, indent + 1);
        }
        break;
    case STMT_FOR:
        indent_print(indent);
        fprintf(stdout, "for (");
        if (s->init_expr != NULL)
            expr_print(s->init_expr, 0);
        fprintf(stdout, ";");
        if (s->expr != NULL)
            expr_print(s->expr, 0);
        fprintf(stdout, ";");
        if (s->next_expr != NULL)
            expr_print(s->next_expr, 0);
        fprintf(stdout, ") ");
        if (s->body->kind != STMT_BLOCK)
            fprintf(stdout, "\n");
        stmt_print(s->body, indent + 1);
        break;
    case STMT_PRINT:
        indent_print(indent);
        fprintf(stdout, "print");
        if (s->expr != NULL) {
            fprintf(stdout, " ");
            expr_print(s->expr, 0);
        }
        fprintf(stdout, ";\n");
        break;
    case STMT_RETURN:
        indent_print(indent);
        fprintf(stdout, "return");
        if (s->expr != NULL) {
            fprintf(stdout, " ");
            expr_print(s->expr, 0);
        }
        fprintf(stdout, ";\n");
        break;
    default:
        break;
    }


    stmt_print(s->next, indent);
}
