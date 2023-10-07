#ifndef SCANNER_H
#define SCANNER_H

#include "parser.h"
#include <stdio.h>

extern FILE *yyin;
extern char *yytext;
extern int yylex();

extern char *tokstr[];

#endif
