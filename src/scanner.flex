%{
#include "token.h"
%}

%%

[ \t\n]+ /* Ignore whitespace. */

array { return TOKEN_ARRAY; }
auto { return TOKEN_AUTO; }
boolean { return TOKEN_BOOLEAN; }
char { return TOKEN_CHAR; }
else { return TOKEN_ELSE; }
false { return TOKEN_FALSE; }
float { return TOKEN_FLOAT; }
for { return TOKEN_FOR; }
function { return TOKEN_FUNCTION; }
if { return TOKEN_IF; }
integer { return TOKEN_INTEGER; }
print { return TOKEN_PRINT; }
return { return TOKEN_RETURN; }
string { return TOKEN_STRING; }
true { return TOKEN_TRUE; }
void { return TOKEN_VOID; }
while { return TOKEN_WHILE; }

\+\+ { return TOKEN_INCREMENT; }
-- { return TOKEN_DECREMENT; }
! { return TOKEN_NOT; }
\^ { return TOKEN_EXPONENTIATION; }
\* { return TOKEN_MULTIPLICATION; }
\/ { return TOKEN_DIVISION; }
% { return TOKEN_MODULO; }
\+ { return TOKEN_ADDITION; }
- { return TOKEN_SUBTRACTION; }
\< { return TOKEN_LESSER; }
\<= { return TOKEN_LESSER_EQUAL; }
> { return TOKEN_GREATER; }
>= { return TOKEN_GREATER_EQUAL; }
== { return TOKEN_EQUALITY; }
!= { return TOKEN_INEQUALITY; }
&& { return TOKEN_AND; }
\|\| { return TOKEN_OR; }
= { return TOKEN_ASSIGNMENT; }

\{ { return TOKEN_BRACE_OPEN; }
\} { return TOKEN_BRACE_CLOSE; }
\( { return TOKEN_PAREN_OPEN; }
\) { return TOKEN_PAREN_CLOSE; }
\[ { return TOKEN_BRACK_OPEN; }
\] { return TOKEN_BRACK_CLOSE; }
: { return TOKEN_COLON; }
; { return TOKEN_SEMICOLON; }
, { return TOKEN_COMMA; }

\/\/[^\n]*\n { return TOKEN_COMMENT_SINGLE; }
\/\*(\*+[^*/]|[^*])*\*+\/ { return TOKEN_COMMENT_MULTI; }

-?[0-9]+ { return TOKEN_INTEGER_LITERAL; }
-?([0-9]*\.[0-9]+|[0-9]+(e|E)-?[0-9]+) { return TOKEN_FLOAT_LITERAL; }
%{ /*NEED STRING AND CHARACTER LITERALS */ %}

[_a-zA-Z][_a-zA-Z0-9]* { return TOKEN_IDENTIFIER; }

%%

int yywrap() { return 1; }