%{

#include "encode.h"
#include "parser.h"

%}

%%

[ \t\r\n]+                  /* Ignore whitespace. */
\/\/[^\n]*\n                /* Ignore single-line comments. */
\/\*(\*+[^*/]|[^*])*\*+\/   /* Ignore multi-line comments. */

<<EOF>>     { return TOKEN_EOF; }

array       { return TOKEN_ARRAY; }
auto        { return TOKEN_AUTO; }
boolean     { return TOKEN_BOOLEAN; }
char        { return TOKEN_CHAR; }
else        { return TOKEN_ELSE; }
false       { return TOKEN_FALSE; }
float       { return TOKEN_FLOAT; }
for         { return TOKEN_FOR; }
function    { return TOKEN_FUNCTION; }
if          { return TOKEN_IF; }
integer     { return TOKEN_INTEGER; }
print       { return TOKEN_PRINT; }
return      { return TOKEN_RETURN; }
string      { return TOKEN_STRING; }
true        { return TOKEN_TRUE; }
void        { return TOKEN_VOID; }
while       { return TOKEN_WHILE; }

\+\+        { return TOKEN_INC; }
--          { return TOKEN_DEC; }
!           { return TOKEN_NOT; }
\^          { return TOKEN_EXP; }
\*          { return TOKEN_MULT; }
\/          { return TOKEN_DIV; }
%           { return TOKEN_MOD; }
\+          { return TOKEN_PLUS; }
-           { return TOKEN_MINUS; }
\<          { return TOKEN_LT; }
\<=         { return TOKEN_LTE; }
>           { return TOKEN_GT; }
>=          { return TOKEN_GTE; }
==          { return TOKEN_EQ; }
!=          { return TOKEN_NOTEQ; }
&&          { return TOKEN_AND; }
\|\|        { return TOKEN_OR; }
=           { return TOKEN_ASSIGN; }

\{          { return TOKEN_LBRACE; }
\}          { return TOKEN_RBRACE; }
\(          { return TOKEN_LPAREN; }
\)          { return TOKEN_RPAREN; }
\[          { return TOKEN_LBRACK; }
\]          { return TOKEN_RBRACK; }
:           { return TOKEN_COLON; }
;           { return TOKEN_SEMICOLON; }
,           { return TOKEN_COMMA; }


[0-9]+ {
    if (integer_decode(yytext, NULL) != 0)
        return TOKEN_INVALID_ANY;

    return TOKEN_INTEGERLIT;
}

([0-9]+(e|E)(\+|-)?[0-9]+)|([0-9]*\.[0-9]+((e|E)(\+|-)?[0-9]+)?) {
    if (float_decode(yytext, NULL) != 0)
        return TOKEN_INVALID_ANY;

    return TOKEN_FLOATLIT;
}

'\\''|'[^']*' {
    if (char_decode(yytext, NULL) != 0)
        return TOKEN_INVALID_ANY;

    return TOKEN_CHARLIT;
}

\"(\\\"|[^"\n])*\" {
    if (string_decode(yytext, NULL) != 0)
        return TOKEN_INVALID_ANY;

    return TOKEN_STRINGLIT;
}

[_a-zA-Z][_a-zA-Z0-9]* { 
    if (yyleng > 255)
        return TOKEN_INVALID_LONG;

    return TOKEN_IDENT;
}

[0-9][_a-zA-Z0-9]+  { return TOKEN_INVALID_NUMIDENT; }
\/\*([^/*]|\*[^/])* { return TOKEN_INVALID_OPENCOM; }
.                   { return TOKEN_INVALID_ANY; }

%%

int yywrap() {
    return 1;
}
