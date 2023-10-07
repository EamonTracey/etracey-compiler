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
%token TOKEN_INCREMENT 
%token TOKEN_DECREMENT 
%token TOKEN_NOT 
%token TOKEN_EXPONENTIATION 
%token TOKEN_MULTIPLICATION 
%token TOKEN_DIVISION 
%token TOKEN_MODULO 
%token TOKEN_ADDITION 
%token TOKEN_SUBTRACTION 
%token TOKEN_LESSER 
%token TOKEN_LESSER_EQUAL 
%token TOKEN_GREATER 
%token TOKEN_GREATER_EQUAL 
%token TOKEN_EQUALITY 
%token TOKEN_INEQUALITY 
%token TOKEN_AND 
%token TOKEN_OR 
%token TOKEN_ASSIGNMENT 

/* Punctuation. */
%token TOKEN_BRACE_OPEN 
%token TOKEN_BRACE_CLOSE 
%token TOKEN_PAREN_OPEN 
%token TOKEN_PAREN_CLOSE 
%token TOKEN_BRACK_OPEN 
%token TOKEN_BRACK_CLOSE 
%token TOKEN_COLON 
%token TOKEN_SEMICOLON 
%token TOKEN_COMMA 

/* Comments. */
%token TOKEN_COMMENT_SINGLE 
%token TOKEN_COMMENT_MULTI 

/* Literals. */
%token TOKEN_INTEGER_LITERAL 
%token TOKEN_FLOAT_LITERAL 
%token TOKEN_CHAR_LITERAL 
%token TOKEN_STRING_LITERAL 

/* Identifier. */
%token TOKEN_IDENTIFIER 

/* Catch invalid tokens. */
%token TOKEN_IDENTL 
%token TOKEN_INVALID 
%token TOKEN_IDENTN 
%token TOKEN_UNTCOM

%{

#include <stdio.h>

extern char *yytext;
extern int yylex();
extern int yyerror( char *str );

%}

%%


program: decllist { return 0; }
       |
       ;

/* Expressions. */
expr0: expr0 TOKEN_ASSIGNMENT expr1
     | expr1
     ;

expr1: expr1 TOKEN_OR expr2
     | expr2
     ;

expr2: expr2 TOKEN_AND expr3
     | expr3
     ;

expr3: expr3 TOKEN_LESSER expr4
     | expr3 TOKEN_LESSER_EQUAL expr4
     | expr3 TOKEN_GREATER expr4
     | expr3 TOKEN_GREATER_EQUAL expr4
     | expr3 TOKEN_EQUALITY expr4
     | expr3 TOKEN_INEQUALITY expr4
     | expr4
     ;

expr4: expr4 TOKEN_ADDITION expr5
     | expr4 TOKEN_SUBTRACTION expr5
     | expr5
     ;

expr5: expr5 TOKEN_MULTIPLICATION expr6
     | expr5 TOKEN_DIVISION expr6
     | expr5 TOKEN_MODULO expr6
     | expr6
     ;

expr6: expr6 TOKEN_EXPONENTIATION expr7
     | expr7
     ;

expr7: TOKEN_SUBTRACTION expr8
     | TOKEN_NOT expr8
     | expr8
     ;

expr8: expr9 TOKEN_INCREMENT
     | expr9 TOKEN_DECREMENT
     | expr9
     ;

expr9: TOKEN_PAREN_OPEN expr0 TOKEN_PAREN_CLOSE
     | TOKEN_IDENTIFIER TOKEN_BRACK_OPEN expr0 TOKEN_BRACK_CLOSE
     | TOKEN_IDENTIFIER TOKEN_PAREN_OPEN optexprlist TOKEN_PAREN_CLOSE
     | expratom
     ;

expratom: TOKEN_INTEGER_LITERAL
        | TOKEN_FLOAT_LITERAL
        | TOKEN_CHAR_LITERAL
        | TOKEN_STRING_LITERAL
        | TOKEN_IDENTIFIER
        | TOKEN_TRUE
        | TOKEN_FALSE
        | TOKEN_BRACE_OPEN exprlist TOKEN_BRACE_CLOSE
        ;

optexpr: expr0
       |
       ;

exprlist: expr0 TOKEN_COMMA exprlist
        | expr0
        ;

optexprlist: exprlist
           |
           ; 

/* Types. */
type: TOKEN_INTEGER
    | TOKEN_FLOAT
    | TOKEN_BOOLEAN
    | TOKEN_CHAR
    | TOKEN_STRING
    | TOKEN_VOID
    | TOKEN_ARRAY TOKEN_BRACK_OPEN expr0 TOKEN_BRACK_CLOSE type
    | TOKEN_FUNCTION type TOKEN_PAREN_OPEN optparamlist TOKEN_PAREN_CLOSE
    ;

param: TOKEN_IDENTIFIER TOKEN_COLON type
     ;

paramlist: param TOKEN_COMMA paramlist
         | param
         ;

optparamlist: paramlist
            | 
            ;


/* Statements. */
stmt: decl
    | expr0 TOKEN_SEMICOLON
    | TOKEN_IF TOKEN_PAREN_OPEN expr0 TOKEN_PAREN_CLOSE stmt
    | TOKEN_FOR TOKEN_PAREN_OPEN optexpr TOKEN_SEMICOLON optexpr TOKEN_SEMICOLON optexpr TOKEN_PAREN_CLOSE stmt
    | TOKEN_PRINT optexprlist TOKEN_SEMICOLON
    | TOKEN_RETURN optexpr TOKEN_SEMICOLON
    | TOKEN_BRACE_OPEN optstmtlist TOKEN_BRACE_CLOSE
    ;

stmtlist: stmt stmtlist
        | stmt
        ;

optstmtlist: stmtlist
           |
           ;

/* Declarations. */
decl: TOKEN_IDENTIFIER TOKEN_COLON type TOKEN_SEMICOLON
    | TOKEN_IDENTIFIER TOKEN_COLON type TOKEN_ASSIGNMENT expr0 TOKEN_SEMICOLON
    | TOKEN_IDENTIFIER TOKEN_COLON type TOKEN_ASSIGNMENT TOKEN_BRACE_OPEN optstmtlist TOKEN_BRACE_CLOSE
    ;

decllist: decl decllist
        | decl
        ;

%%

int yyerror(char *str)
{
	printf("parse error: %s\n",str);
	return 0;
}
