/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_INCLUDE_PARSER_H_INCLUDED
# define YY_YY_INCLUDE_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    TOKEN_EOF = 258,
    TOKEN_ARRAY = 259,
    TOKEN_AUTO = 260,
    TOKEN_BOOLEAN = 261,
    TOKEN_CHAR = 262,
    TOKEN_ELSE = 263,
    TOKEN_FALSE = 264,
    TOKEN_FLOAT = 265,
    TOKEN_FOR = 266,
    TOKEN_FUNCTION = 267,
    TOKEN_IF = 268,
    TOKEN_INTEGER = 269,
    TOKEN_PRINT = 270,
    TOKEN_RETURN = 271,
    TOKEN_STRING = 272,
    TOKEN_TRUE = 273,
    TOKEN_VOID = 274,
    TOKEN_WHILE = 275,
    TOKEN_INC = 276,
    TOKEN_DEC = 277,
    TOKEN_NOT = 278,
    TOKEN_EXP = 279,
    TOKEN_MULT = 280,
    TOKEN_DIV = 281,
    TOKEN_MOD = 282,
    TOKEN_PLUS = 283,
    TOKEN_MINUS = 284,
    TOKEN_LT = 285,
    TOKEN_LTE = 286,
    TOKEN_GT = 287,
    TOKEN_GTE = 288,
    TOKEN_EQ = 289,
    TOKEN_NOTEQ = 290,
    TOKEN_AND = 291,
    TOKEN_OR = 292,
    TOKEN_ASSIGN = 293,
    TOKEN_LBRACE = 294,
    TOKEN_RBRACE = 295,
    TOKEN_LPAREN = 296,
    TOKEN_RPAREN = 297,
    TOKEN_LBRACK = 298,
    TOKEN_RBRACK = 299,
    TOKEN_COLON = 300,
    TOKEN_SEMICOLON = 301,
    TOKEN_COMMA = 302,
    TOKEN_INTEGERLIT = 303,
    TOKEN_FLOATLIT = 304,
    TOKEN_CHARLIT = 305,
    TOKEN_STRINGLIT = 306,
    TOKEN_IDENT = 307,
    TOKEN_INVALID_LONG = 308,
    TOKEN_INVALID_NUMIDENT = 309,
    TOKEN_INVALID_OPENCOM = 310,
    TOKEN_INVALID_ANY = 311
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 68 "src/parser.bison" /* yacc.c:1909  */

	struct decl *decl;
	struct expr *expr;
    struct param_list *param_list;
	struct stmt *stmt;
    struct type *type;

    char *ident;

#line 121 "include/parser.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_INCLUDE_PARSER_H_INCLUDED  */
