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

%{

#include <stdio.h>

extern char *yytext;
extern int yylex();
extern void yyerror(char const *);

%}

%%

/* A B-Minor file is a sequence of declarations. Empty files are valid. */
program: decl_seq_opt TOKEN_EOF { return 0; }
       ;

/* Expressions. */

/* Top-level expression. */
expr: expr0
    ;

/* Assignment operator. */
expr0: expr0 TOKEN_ASSIGN expr1
     | expr1
     ;

/* Or operator. */
expr1: expr1 TOKEN_OR expr2
     | expr2
     ;

/* And operator. */
expr2: expr2 TOKEN_AND expr3
     | expr3
     ;

/* Less-than, less-than-or-equal, greater-than, greater-than-or-equal, equal-to, not-equal-to operators. */
expr3: expr3 TOKEN_LT expr4
     | expr3 TOKEN_LTE expr4
     | expr3 TOKEN_GT expr4
     | expr3 TOKEN_GTE expr4
     | expr3 TOKEN_EQ expr4
     | expr3 TOKEN_NOTEQ expr4
     | expr4
     ;

/* Addition and subtraction operators. */
expr4: expr4 TOKEN_PLUS expr5
     | expr4 TOKEN_MINUS expr5
     | expr5
     ;

/* Multiplication, division, and modulo operators. */
expr5: expr5 TOKEN_MULT expr6
     | expr5 TOKEN_DIV expr6
     | expr5 TOKEN_MOD expr6
     | expr6
     ;

/* Exponentiation operator. */
expr6: expr6 TOKEN_EXP expr7
     | expr7
     ;

/* Unary-positive, unary-negative, and logical-not operators */
expr7: TOKEN_PLUS expr8
     | TOKEN_MINUS expr8
     | TOKEN_NOT expr8
     | expr8
     ;

/* Increment and decrement operators. */
expr8: expr9 TOKEN_INC
     | expr9 TOKEN_DEC
     | expr9
     ;

/* Paranthetical grouping, subscripting, and function calls. */
expr9: TOKEN_LPAREN expr TOKEN_RPAREN
     | expr9 TOKEN_LBRACK expr TOKEN_RBRACK
     | TOKEN_IDENT TOKEN_LPAREN expr_seq_opt TOKEN_RPAREN
     | expr_atom
     ;

/* Atomic expressions. */
expr_atom: TOKEN_IDENT
         | TOKEN_INTEGERLIT
         | TOKEN_FLOATLIT
         | TOKEN_CHARLIT
         | TOKEN_STRINGLIT
         | TOKEN_TRUE
         | TOKEN_FALSE
         | TOKEN_LBRACE expr_seq TOKEN_RBRACE
         ;

/* Optional expression. */
expr_opt: expr
        |
        ;

/* Comma-separated expression sequence. */
expr_seq: expr TOKEN_COMMA expr_seq
        | expr
        ;

/* Optional expression sequence. */
expr_seq_opt: expr_seq
            |
            ;

/* Types. */

/* Top-level type. */
type: type_atom
    | TOKEN_VOID
    | TOKEN_ARRAY TOKEN_LBRACK expr_opt TOKEN_RBRACK type
    | TOKEN_FUNCTION type TOKEN_LPAREN param_seq_opt TOKEN_RPAREN
    ;

/* Atomic types. */
type_atom: TOKEN_INTEGER
         | TOKEN_FLOAT
         | TOKEN_BOOLEAN
         | TOKEN_CHAR
         | TOKEN_STRING

/* Parameter. */
param: TOKEN_IDENT TOKEN_COLON type
     ;

/* Comma-separated parameter sequence. */
param_seq: param TOKEN_COMMA param_seq
          | param
          ;

/* Optional parameter sequence. */
param_seq_opt: param_seq
              |
              ;

/* Statements. */

/* Top-level statement. */
stmt: simple_stmt
    | if_stmt
    | for_stmt
    ;

/* Simple statement. */
simple_stmt: decl
           | expr TOKEN_SEMICOLON
           | TOKEN_RETURN expr_opt TOKEN_SEMICOLON
           | TOKEN_PRINT expr_seq_opt TOKEN_SEMICOLON
           | TOKEN_LBRACE stmt_seq_opt TOKEN_RBRACE
           ;

/* If statement which may dangle. */
if_stmt: TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt
       | TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt_closed TOKEN_ELSE stmt
       ;

/* If statement which may not dangle. */
if_closed: TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt_closed TOKEN_ELSE stmt_closed
         ;

/* For statement which may dangle. */
for_stmt: TOKEN_FOR TOKEN_LPAREN expr_opt TOKEN_SEMICOLON expr_opt TOKEN_SEMICOLON expr_opt TOKEN_RPAREN stmt
        ;

/* For statement which may not dangle. */
for_closed: TOKEN_FOR TOKEN_LPAREN expr_opt TOKEN_SEMICOLON expr_opt TOKEN_SEMICOLON expr_opt TOKEN_RPAREN stmt_closed
          ;

/* Statement which may not dangle. */
stmt_closed: simple_stmt
           | if_closed
           | for_closed
           ;

/* Statement sequence. */
stmt_seq: stmt stmt_seq
        | stmt
        ;

/* Optional statement sequence. */
stmt_seq_opt: stmt_seq
            |
            ;

/* Declarations. */

/* Top-level declaration. */
decl: TOKEN_IDENT TOKEN_COLON type TOKEN_SEMICOLON
    | TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN expr TOKEN_SEMICOLON
    | TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN TOKEN_LBRACE stmt_seq_opt TOKEN_RBRACE
    ;

/* Declaration sequence. */
decl_seq: decl decl_seq
        | decl
        ;

decl_seq_opt: decl_seq
            |
            ;

%%

void yyerror(char const *s) {
    fprintf(stderr, "parse error: %s.\n", s);
}
