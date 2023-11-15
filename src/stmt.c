#include <stdlib.h>
#include <stdio.h>

#include "decl.h"
#include "expr.h"
#include "indent.h"
#include "scope.h"
#include "stmt.h"

extern int type_errors;

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

void stmt_resolve(struct stmt *s) {
    if (s == NULL)
        return;
    
    switch (s->kind) {
    case STMT_DECL:
        decl_resolve(s->decl);
        break;
    case STMT_IF_ELSE:
        expr_resolve(s->expr);
        scope_enter();
        stmt_resolve(s->body);
        scope_exit();
        scope_enter();
        stmt_resolve(s->else_body);
        scope_exit();
        break;
    case STMT_FOR:
        expr_resolve(s->init_expr);
        expr_resolve(s->expr);
        expr_resolve(s->next_expr);
        scope_enter();
        stmt_resolve(s->body);
        scope_exit();
        break;
    case STMT_EXPR:
    case STMT_PRINT:
    case STMT_RETURN:
        expr_resolve(s->expr);
        break;
    case STMT_BLOCK:
        scope_enter();
        stmt_resolve(s->body);
        scope_exit();
        break;
    }

    stmt_resolve(s->next);
}

void stmt_typecheck(struct stmt *s, struct type *ret) {
    if (s == NULL)
        return;

    struct type *t;

    switch (s->kind) {
    case STMT_DECL:
        decl_typecheck(s->decl);
        break;
    case STMT_IF_ELSE:
        t = expr_typecheck(s->expr);
        if (t->kind != TYPE_BOOLEAN) {
            ++type_errors;
            fprintf(stdout, "type error: if condition cannot be type ");
            type_print(t);
            fprintf(stdout, " (");
            expr_print(s->expr, 0);
            fprintf(stdout, ").\n");
        }
        stmt_typecheck(s->body, ret);
        stmt_typecheck(s->else_body, ret);
        break;
    case STMT_FOR:
        expr_typecheck(s->init_expr);
        t = expr_typecheck(s->expr);
        if (t->kind != TYPE_BOOLEAN) {
            ++type_errors;
            fprintf(stdout, "type error: for condition cannot be type ");
            type_print(t);
            fprintf(stdout, " (");
            expr_print(s->expr, 0);
            fprintf(stdout, ").\n");
        }
        expr_typecheck(s->next_expr);
        break;
    case STMT_EXPR:
        expr_typecheck(s->expr);
        break;
    case STMT_PRINT:
        break;
    case STMT_RETURN:
        t = expr_typecheck(s->expr);
        if (t == NULL) {
            if (ret->kind != TYPE_VOID) {
                ++type_errors;
                fprintf(stdout, "type error: must return a value in non-void function.\n");
            }
        } else if (ret->kind == TYPE_VOID) {
            ++type_errors;
            fprintf(stdout, "type error: cannot return type ");
            type_print(t);
            fprintf(stdout, " (");
            expr_print(s->expr, 0);
            fprintf(stdout, ") from void function.\n");
        } else if (!type_equals(t, ret)) {
            ++type_errors;
            fprintf(stdout, "type error: cannot return type ");
            type_print(t);
            fprintf(stdout, " (");
            expr_print(s->expr, 0);
            fprintf(stdout, ") from function with return type ");
            type_print(ret);
            fprintf(stdout, ".\n");
        }
        break;
    case STMT_BLOCK:
        stmt_typecheck(s->body, ret);
        break;
    }

    stmt_typecheck(s->next, ret);
}
