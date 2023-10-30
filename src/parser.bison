%token TOKEN_EOF

/* Keywords. */
%token TOKEN_ARRAY
%token TOKEN_AUTO
%token TOKEN_BOOLEAN
%token TOKEN_CHAR
%token TOKEN_ELSE
%token TOKEN_FALSE
%token TOKEN_FLOAT
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_INTEGER
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_STRING
%token TOKEN_TRUE
%token TOKEN_VOID
%token TOKEN_WHILE

/* Operators. */
%token TOKEN_INC
%token TOKEN_DEC
%token TOKEN_NOT
%token TOKEN_EXP
%token TOKEN_MULT
%token TOKEN_DIV
%token TOKEN_MOD
%token TOKEN_PLUS
%token TOKEN_MINUS
%token TOKEN_LT
%token TOKEN_LTE
%token TOKEN_GT
%token TOKEN_GTE
%token TOKEN_EQ
%token TOKEN_NOTEQ
%token TOKEN_AND
%token TOKEN_OR
%token TOKEN_ASSIGN

/* Punctuation. */
%token TOKEN_LBRACE
%token TOKEN_RBRACE
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_LBRACK
%token TOKEN_RBRACK
%token TOKEN_COLON
%token TOKEN_SEMICOLON
%token TOKEN_COMMA

/* Literals. */
%token TOKEN_INTEGERLIT
%token TOKEN_FLOATLIT
%token TOKEN_CHARLIT
%token TOKEN_STRINGLIT

/* Identifier. */
%token TOKEN_IDENT

/* Catch invalid tokens. */
%token TOKEN_INVALID_LONG
%token TOKEN_INVALID_NUMIDENT
%token TOKEN_INVALID_OPENCOM
%token TOKEN_INVALID_ANY

%union {
	struct decl *decl;
	struct expr *expr;
    struct param_list *param_list;
	struct stmt *stmt;
    struct type *type;

    char *ident;
};

%type <decl> program decl decl_seq decl_seq_opt
%type <expr> expr expr0 expr1 expr2 expr3 expr4 expr5 expr6 expr7 expr8 expr9 expr_atom expr_opt expr_seq expr_seq_opt
%type <param_list> param param_seq param_seq_opt
%type <stmt> stmt simple_stmt if_stmt if_closed for_stmt for_closed stmt_closed stmt_seq stmt_seq_opt
%type <type> type type_atom
%type <ident> ident

%{

#include <stdio.h>
#include <string.h>

#include "decl.h"
#include "encode.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "type.h"

extern char *yytext;
extern int yylex();
extern void yyerror(char const *);

struct decl *ast;
int literal_value;

%}

%%

/* A B-Minor file is a sequence of declarations. Empty files are valid. */
program: decl_seq_opt TOKEN_EOF
         { ast = $1; return 0; }
       ;

/* Expressions. */

/* Top-level expression. */
expr: expr0
      { $$ = $1; }
    ;

/* Assignment operator. */
expr0: expr0 TOKEN_ASSIGN expr1
       { $$ = expr_create(EXPR_PLUS, $1, $3); }
     | expr1
       { $$ = $1; }
     ;

/* Or operator. */
expr1: expr1 TOKEN_OR expr2
       { $$ = expr_create(EXPR_OR, $1, $3); }
     | expr2
       { $$ = $1; }
     ;

/* And operator. */
expr2: expr2 TOKEN_AND expr3
       { $$ = expr_create(EXPR_AND, $1, $3); }
     | expr3
       { $$ = $1; }
     ;

/* Less-than, less-than-or-equal, greater-than, greater-than-or-equal, equal-to, not-equal-to operators. */
expr3: expr3 TOKEN_LT expr4
       { $$ = expr_create(EXPR_LT, $1, $3); }
     | expr3 TOKEN_LTE expr4
       { $$ = expr_create(EXPR_LTE, $1, $3); }
     | expr3 TOKEN_GT expr4
       { $$ = expr_create(EXPR_GT, $1, $3); }
     | expr3 TOKEN_GTE expr4
       { $$ = expr_create(EXPR_GTE, $1, $3); }
     | expr3 TOKEN_EQ expr4
       { $$ = expr_create(EXPR_EQ, $1, $3); }
     | expr3 TOKEN_NOTEQ expr4
       { $$ = expr_create(EXPR_NOTEQ, $1, $3); }
     | expr4
       { $$ = $1; }
     ;

/* Addition and subtraction operators. */
expr4: expr4 TOKEN_PLUS expr5
       { $$ = expr_create(EXPR_NEG, $1, $3); }
     | expr4 TOKEN_MINUS expr5
       { $$ = expr_create(EXPR_POS, $1, $3); }
     | expr5
       { $$ = $1; }
     ;

/* Multiplication, division, and modulo operators. */
expr5: expr5 TOKEN_MULT expr6
       { $$ = expr_create(EXPR_MULT, $1, $3); }
     | expr5 TOKEN_DIV expr6
       { $$ = expr_create(EXPR_DIV, $1, $3); }
     | expr5 TOKEN_MOD expr6
       { $$ = expr_create(EXPR_MOD, $1, $3); }
     | expr6
       { $$ = $1; }
     ;

/* Exponentiation operator. */
expr6: expr6 TOKEN_EXP expr7
       { $$ = expr_create(EXPR_EXP, $1, $3); }
     | expr7
       { $$ = $1; }
     ;

/* Unary-positive, unary-negative, and logical-not operators */
expr7: TOKEN_PLUS expr8
       { $$ = expr_create(EXPR_POS, $2, NULL); }
     | TOKEN_MINUS expr8
       { $$ = expr_create(EXPR_NEG, $2, NULL); }
     | TOKEN_NOT expr8
       { $$ = expr_create(EXPR_OR, $2, NULL); }
     | expr8
       { $$ = $1; }
     ;

/* Increment and decrement operators. */
expr8: expr9 TOKEN_INC
       { $$ = expr_create(EXPR_INC, $1, NULL); }
     | expr9 TOKEN_DEC
       { $$ = expr_create(EXPR_DEC, $1, NULL); }
     | expr9
       { $$ = $1; }
     ;

/* Paranthetical grouping, subscripting, and function calls. */
expr9: TOKEN_LPAREN expr TOKEN_RPAREN
       { $$ = $2; }
     | expr9 TOKEN_LBRACK expr TOKEN_RBRACK
       { $$ = expr_create(EXPR_ARRACC, $1, $3); }
     | ident TOKEN_LPAREN expr_seq_opt TOKEN_RPAREN
       { $$ = expr_create(EXPR_FUNCCALL, expr_create_name($1), $3); }
     | expr_atom
       { $$ = $1; }
     ;

/* Atomic expressions. */
expr_atom: ident
           { $$ = expr_create_name(yytext); }
         | TOKEN_INTEGERLIT
           { integer_decode(yytext, &literal_value); $$ = expr_create_integer_literal(literal_value); }
         | TOKEN_FLOATLIT
// IMPLEMENT FLOAT
           { $$ = expr_create_integer_literal(0); }
         | TOKEN_CHARLIT
           { integer_decode(yytext, &literal_value); $$ = expr_create_char_literal(literal_value); }
         | TOKEN_STRINGLIT
           { $$ = expr_create_string_literal(yytext); }
         | TOKEN_TRUE
           { $$ = expr_create_boolean_literal(1); }
         | TOKEN_FALSE
           { $$ = expr_create_integer_literal(0); }
         | TOKEN_LBRACE expr_seq TOKEN_RBRACE
           { $$ = $2; }
         ;

/* Optional expression. */
expr_opt: expr
          { $$ = $1; }
        |
          { $$ = NULL; }
        ;

/* Comma-separated expression sequence. */
expr_seq: expr TOKEN_COMMA expr_seq
// MUST IMPLEMENT EXPR LIST
          { $$ = $1; }
        | expr
          { $$ = $1; }
        ;

/* Optional expression sequence. */
expr_seq_opt: expr_seq
              { $$ = $1; }
            |
              { $$ = NULL; }
            ;

/* Types. */

/* Top-level type. */
type: type_atom
      { $$ = $1; }
    | TOKEN_VOID
      { $$ = type_create(TYPE_VOID, NULL, NULL); }
    | TOKEN_ARRAY TOKEN_LBRACK expr_opt TOKEN_RBRACK type
      { $$ = type_create(TYPE_ARRAY, $5, NULL); }
    | TOKEN_FUNCTION type TOKEN_LPAREN param_seq_opt TOKEN_RPAREN
      { $$ = type_create(TYPE_FUNCTION, $2, $4); }
    ;

/* Atomic types. */
type_atom: TOKEN_INTEGER
           { $$ = type_create(TYPE_INTEGER, NULL, NULL); }
         | TOKEN_FLOAT
           { $$ = type_create(TYPE_INTEGER, NULL, NULL); }
         | TOKEN_BOOLEAN
           { $$ = type_create(TYPE_BOOLEAN, NULL, NULL); }
         | TOKEN_CHAR
           { $$ = type_create(TYPE_CHARACTER, NULL, NULL); }
         | TOKEN_STRING
           { $$ = type_create(TYPE_STRING, NULL, NULL); }

/* Parameter. */
param: ident TOKEN_COLON type
       { $$ = param_list_create($1, $3, NULL); }
     ;

/* Comma-separated parameter sequence. */
param_seq: param TOKEN_COMMA param_seq
           { $$ = $1; $$->next = $3; }
         | param
            { $$ = $1; }
         ;

/* Optional parameter sequence. */
param_seq_opt: param_seq
               { $$ = $1; }
              |
               { $$ = NULL; }
              ;

/* Statements. */

/* Top-level statement. */
stmt: simple_stmt
      { $$ = $1; }
    | if_stmt
      { $$ = $1; }
    | for_stmt
      { $$ = $1; }
    ;

/* Simple statement. */
simple_stmt: decl
             { $$ = stmt_create(STMT_DECL, $1, NULL, NULL, NULL, NULL, NULL, NULL); }
           | expr TOKEN_SEMICOLON
             { $$ = stmt_create(STMT_EXPR, NULL, NULL, $1, NULL, NULL, NULL, NULL); }
           | TOKEN_RETURN expr_opt TOKEN_SEMICOLON
             { $$ = stmt_create(STMT_RETURN, NULL, NULL, $2, NULL, NULL, NULL, NULL); }
           | TOKEN_PRINT expr_seq_opt TOKEN_SEMICOLON
             { $$ = stmt_create(STMT_PRINT, NULL, NULL, $2, NULL, NULL, NULL, NULL); }
           | TOKEN_LBRACE stmt_seq_opt TOKEN_RBRACE
             { $$ = stmt_create(STMT_BLOCK, NULL, NULL, NULL, NULL, $2, NULL, NULL); }
           ;

/* If statement which may dangle. */
if_stmt: TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt
         { $$ = stmt_create(STMT_IF_ELSE, NULL, NULL, $3, NULL, $5, NULL, NULL); }
       | TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt_closed TOKEN_ELSE stmt
         { $$ = stmt_create(STMT_IF_ELSE, NULL, NULL, $3, NULL, $5, $7, NULL); }
       ;

/* If statement which may not dangle. */
if_closed: TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt_closed TOKEN_ELSE stmt_closed
           { $$ = stmt_create(STMT_IF_ELSE, NULL, NULL, $3, NULL, $5, $7, NULL); }
         ;

/* For statement which may dangle. */
for_stmt: TOKEN_FOR TOKEN_LPAREN expr_opt TOKEN_SEMICOLON expr_opt TOKEN_SEMICOLON expr_opt TOKEN_RPAREN stmt
          { $$ = stmt_create(STMT_FOR, NULL, $3, $5, $7, $9, NULL, NULL); }
        ;

/* For statement which may not dangle. */
for_closed: TOKEN_FOR TOKEN_LPAREN expr_opt TOKEN_SEMICOLON expr_opt TOKEN_SEMICOLON expr_opt TOKEN_RPAREN stmt_closed
            { $$ = stmt_create(STMT_FOR, NULL, $3, $5, $7, $9, NULL, NULL); }
          ;

/* Statement which may not dangle. */
stmt_closed: simple_stmt
             { $$ = $1; }
           | if_closed
             { $$ = $1; }
           | for_closed
             { $$ = $1; }
           ;

/* Statement sequence. */
stmt_seq: stmt stmt_seq
          { $$ = $1; $$ = $1->next; }
        | stmt
          { $$ = $1; }
        ;

/* Optional statement sequence. */
stmt_seq_opt: stmt_seq
              { $$ = $1; }
            |
              { $$ = NULL; }
            ;

/* Declarations. */

/* Top-level declaration. */
decl: ident TOKEN_COLON type TOKEN_SEMICOLON
      { $$ = decl_create($1, $3, NULL, NULL, NULL); }
    | ident TOKEN_COLON type TOKEN_ASSIGN expr TOKEN_SEMICOLON
      { $$ = decl_create($1, $3, $5, NULL, NULL); }
    | ident TOKEN_COLON type TOKEN_ASSIGN TOKEN_LBRACE stmt_seq_opt TOKEN_RBRACE
      { $$ = decl_create($1, $3, NULL, $6, NULL); }
    ;

/* Declaration sequence. */
decl_seq: decl decl_seq
          { $$ = $1; $$->next = $2; }
        | decl
          { $$ = $1; }
        ;

decl_seq_opt: decl_seq
              { $$ = $1; }
            |
              { $$ = NULL; }
            ;

/* Identifiers. */

ident: TOKEN_IDENT
       { $$ = strdup(yytext); }

%%

void yyerror(char const *s) {
    fprintf(stderr, "parse error: %s.\n", s);
}
