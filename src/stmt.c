#include <stdlib.h>
#include <stdio.h>

#include "decl.h"
#include "expr.h"
#include "indent.h"
#include "label.h"
#include "scope.h"
#include "scratch.h"
#include "stmt.h"
#include "type.h"

extern int type_errors;
extern struct symbol *codegen_func_symbol;

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
    struct expr *e;

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
        if (t != NULL && t->kind != TYPE_BOOLEAN) {
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
        e = s->expr;
        while (e != NULL) {
            t = expr_typecheck(e->left);
            if (!type_is_atomic(t)) {
                ++type_errors;
                fprintf(stdout, "type error: cannot pass non-atomic type ");
                type_print(t);
                fprintf(stdout, " (");
                expr_print(e->left, 0);
                fprintf(stdout, ") to print statement.\n");
            }
            e = e->right;
        }
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

void stmt_codegen(struct stmt *s) {
    if (s == NULL)
        return;

    int else_label;
    int top_label;
    int done_label;
    struct expr *elist;

    int r10_before;
    int r11_before;

    switch (s->kind) {
    case STMT_DECL:
        decl_codegen(s->decl);
        break;
    case STMT_IF_ELSE:
        else_label = label_create();
        done_label = label_create();
        expr_codegen(s->expr);
        fprintf(stdout, "    cmpq $0, %s\n", scratch_name(s->expr->reg));
        fprintf(stdout, "    je %s\n", label_name(else_label));
        scratch_free(s->expr->reg);
        stmt_codegen(s->body);
        fprintf(stdout, "    jmp %s\n", label_name(done_label));
        fprintf(stdout, "%s:\n", label_name(else_label));
        stmt_codegen(s->else_body);
        fprintf(stdout, "%s:\n", label_name(done_label));
        break;
    case STMT_FOR:
        top_label = label_create();
        done_label = label_create();
        if (s->init_expr != NULL)
            expr_codegen(s->init_expr);
        fprintf(stdout, "%s:\n", label_name(top_label));
        if (s->expr != NULL) {
            expr_codegen(s->expr);
            fprintf(stdout, "    cmpq $0, %s\n", scratch_name(s->expr->reg));
            fprintf(stdout, "    je %s\n", label_name(done_label));
        }
        stmt_codegen(s->body);
        if (s->next_expr != NULL)
            expr_codegen(s->next_expr);
        fprintf(stdout, "    jmp %s\n", label_name(top_label));
        fprintf(stdout, "%s:\n", label_name(done_label));
        break;
    case STMT_EXPR:
        expr_codegen(s->expr);
        scratch_free(s->expr->reg);
        break;
    case STMT_PRINT:
        elist = s->expr;
        while (elist != NULL) {
            expr_codegen(elist->left);
            /* call linked print function. */
            fprintf(stdout, "    movq %s, %%rdi\n", scratch_name(elist->left->reg));
            fprintf(stdout, "    pushq %%r10\n");
            fprintf(stdout, "    pushq %%r11\n");
            r10_before = scratch_check(1); scratch_free(1);
            r11_before = scratch_check(2); scratch_free(2);
            if (expr_typecheck(elist->left)->kind == TYPE_INTEGER)
                fprintf(stdout, "    call print_integer\n");
            else if (expr_typecheck(elist->left)->kind == TYPE_BOOLEAN)
                fprintf(stdout, "    call print_boolean\n");
            else if (expr_typecheck(elist->left)->kind == TYPE_CHARACTER)
                fprintf(stdout, "    call print_character\n");
            else if (expr_typecheck(elist->left)->kind == TYPE_STRING)
                fprintf(stdout, "    call print_string\n");
            else {
                /* TODO implement printing floats? */
                exit(1);
            }
            fprintf(stdout, "    popq %%r11\n");
            fprintf(stdout, "    popq %%r10\n");
            scratch_set(1, r10_before);
            scratch_set(2, r11_before);
            scratch_free(elist->left->reg);
            elist = elist->right;
        }
        break;
    case STMT_RETURN:
        expr_codegen(s->expr);
        fprintf(stdout, "    movq %s, %%rax\n", scratch_name(s->expr->reg));
        fprintf(stdout, "    jmp .%s_epilogue\n", codegen_func_symbol->name);
        scratch_free(s->expr->reg);
        break;
    case STMT_BLOCK:
        stmt_codegen(s->body);
        break;
    }

    stmt_codegen(s->next);
}
