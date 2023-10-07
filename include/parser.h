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
    TOKEN_ARRAY = 258,
    TOKEN_AUTO = 259,
    TOKEN_BOOLEAN = 260,
    TOKEN_CHAR = 261,
    TOKEN_ELSE = 262,
    TOKEN_FALSE = 263,
    TOKEN_FLOAT = 264,
    TOKEN_FOR = 265,
    TOKEN_FUNCTION = 266,
    TOKEN_IF = 267,
    TOKEN_INTEGER = 268,
    TOKEN_PRINT = 269,
    TOKEN_RETURN = 270,
    TOKEN_STRING = 271,
    TOKEN_TRUE = 272,
    TOKEN_VOID = 273,
    TOKEN_WHILE = 274,
    TOKEN_INCREMENT = 275,
    TOKEN_DECREMENT = 276,
    TOKEN_NOT = 277,
    TOKEN_EXPONENTIATION = 278,
    TOKEN_MULTIPLICATION = 279,
    TOKEN_DIVISION = 280,
    TOKEN_MODULO = 281,
    TOKEN_ADDITION = 282,
    TOKEN_SUBTRACTION = 283,
    TOKEN_LESSER = 284,
    TOKEN_LESSER_EQUAL = 285,
    TOKEN_GREATER = 286,
    TOKEN_GREATER_EQUAL = 287,
    TOKEN_EQUALITY = 288,
    TOKEN_INEQUALITY = 289,
    TOKEN_AND = 290,
    TOKEN_OR = 291,
    TOKEN_ASSIGNMENT = 292,
    TOKEN_BRACE_OPEN = 293,
    TOKEN_BRACE_CLOSE = 294,
    TOKEN_PAREN_OPEN = 295,
    TOKEN_PAREN_CLOSE = 296,
    TOKEN_BRACK_OPEN = 297,
    TOKEN_BRACK_CLOSE = 298,
    TOKEN_COLON = 299,
    TOKEN_SEMICOLON = 300,
    TOKEN_COMMA = 301,
    TOKEN_COMMENT_SINGLE = 302,
    TOKEN_COMMENT_MULTI = 303,
    TOKEN_INTEGER_LITERAL = 304,
    TOKEN_FLOAT_LITERAL = 305,
    TOKEN_CHAR_LITERAL = 306,
    TOKEN_STRING_LITERAL = 307,
    TOKEN_IDENTIFIER = 308,
    TOKEN_IDENTL = 309,
    TOKEN_INVALID = 310,
    TOKEN_IDENTN = 311,
    TOKEN_UNTCOM = 312
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_INCLUDE_PARSER_H_INCLUDED  */
