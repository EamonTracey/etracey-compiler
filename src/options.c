#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "constants.h"
#include "decl.h"
#include "encode.h"
#include "expr.h"
#include "parser.h"
#include "param_list.h"
#include "scope.h"
#include "stmt.h"
#include "type.h"

extern FILE *yyin;
extern char *yytext;
extern int yylex();
extern int yyparse();

extern struct decl *ast;

extern int verb;

int resolve_errors = 0;
int type_errors = 0;

FILE *codegen_out;

int encode_file(const char *path) {
    char line[2048];

    FILE *stream = fopen(path, "r");
    if (stream == NULL) {
        fprintf(stdout, "error: failed to open file \"%s\".\n", path);
        return -1;
    }

    if (fgets(line, 2048, stream) == NULL) {
        fprintf(stdout, "error: failed to read file \"%s\".\n", path);
        fclose(stream);
        return -1;
    }
    fclose(stream);

    line[strcspn(line, "\n")] = '\0';
    
    char dec[256];
    int ret;
    if ((ret = string_decode(line, dec)) != 0) {
        fprintf(stdout, "encode error: %s\n", strencerr[ret]);
        return -1;
    }

    char reenc[2048];
    string_encode(dec, reenc);

    fprintf(stdout, "%s\n", dec);
    fprintf(stdout, "%s\n", reenc);

    return 0;
}

int scan_file(const char *path) {
    yyin = fopen(path, "r");
    if (yyin == NULL) {
        fprintf(stdout, "error: failed to open file \"%s\".\n", path);
        return -1;
    }

    int ret;
    char errmsg[2048];

    int bmint;
    double bmfloat;
    char bmchar;
    char bmstring[2048];

    int token;
    while ((token = yylex()) != TOKEN_EOF) {
        switch (token) {
        case TOKEN_INTEGERLIT:
            if ((ret = integer_decode(yytext, &bmint)) == 0)
                fprintf(stdout, "%s %d\n", tokstr[token - 258], bmint);
            else {
                snprintf(errmsg, 2048, intencerr[ret], yytext);
                fprintf(stdout, "encode error: %s\n", errmsg);
                return -1;
            }
            break;
        case TOKEN_FLOATLIT:
            if ((ret = float_decode(yytext, &bmfloat)) == 0)
                fprintf(stdout, "%s %lf\n", tokstr[token - 258], bmfloat);
            else {
                snprintf(errmsg, 2048, floatencerr[ret], yytext);
                fprintf(stdout, "encode error: %s\n", errmsg);
                return -1;
            }
            break;
        case TOKEN_CHARLIT:
            if ((ret = char_decode(yytext, &bmchar)) == 0)
                fprintf(stdout, "%s %c\n", tokstr[token - 258], bmchar);
            else {
                fprintf(stdout, "encode error: %s\n", charencerr[ret]);
                return -1;
            }
            break;            
        case TOKEN_STRINGLIT:
            if ((ret = string_decode(yytext, bmstring)) == 0)
                fprintf(stdout, "%s %s\n", tokstr[token - 258], bmstring);
            else {
                fprintf(stdout, "encode error: %s\n", strencerr[ret]);
                return -1;
            }
            break;
        case TOKEN_IDENT:
            fprintf(stdout, "%s %s\n", tokstr[token - 258], yytext);
            break;
        case TOKEN_INVALID_LONG:
            fprintf(stdout, "scan error: identifier must not exceed 255 characters.\n");
            return -1;
        case TOKEN_INVALID_NUMIDENT:
            fprintf(stdout, "scan error: identifier must start with an underscore or letter.\n");
            return -1;
        case TOKEN_INVALID_OPENCOM:
            fprintf(stdout, "scan error: multiline comment must be terminated.\n");
            return -1;
        case TOKEN_INVALID_ANY:
            fprintf(stdout, "scan error: invalid token %s\n", yytext);
            return -1;
        default:
            fprintf(stdout, "%s\n", tokstr[token - 258]);
        }
    }

    fclose(yyin);

    return 0;
}

int parse_file(const char *path) {
    yyin = fopen(path, "r");
    if (yyin == NULL) {
        fprintf(stdout, "error: failed to open file %s.\n", path);
        return -1;
    }

    if (yyparse() != 0)
        return -1;

    fprintf(stdout, "parse successful.\n");
    fclose(yyin);

    return 0;
}

int print_file(const char *path) {
    yyin = fopen(path, "r");
    if (yyin == NULL) {
        fprintf(stdout, "error: failed to open file %s.\n", path);
        return -1;
    }

    if (yyparse() != 0)
        return -1;
    fclose(yyin);

    decl_print(ast, 0);

    return 0;
}

int resolve_file(const char *path) {
    yyin = fopen(path, "r");
    if (yyin == NULL) {
        fprintf(stdout, "error: failed to open file %s.\n", path);
        return -1;
    }

    if (yyparse() != 0)
        return -1;
    fclose(yyin);

    // Enter global scope, resolve program, exit global scope.
    verb = 1;
    scope_enter();
    decl_resolve(ast);
    scope_exit();
    verb = 0;

    return resolve_errors == 0 ? 0 : -1;
}

int typecheck_file(const char *path) {
    yyin = fopen(path, "r");
    if (yyin == NULL) {
        fprintf(stdout, "error: failed to open file %s.\n", path);
        return -1;
    }
    
    // Perform parsing.
    if (yyparse() != 0)
        return -1;
    fclose(yyin);

    // Perform name resolution.
    scope_enter();
    decl_resolve(ast);
    scope_exit();
    if (resolve_errors > 0)
        return -1;

    // Perform typechecking.
    decl_typecheck(ast);

    return type_errors == 0 ? 0 : -1;
}

int codegen_file(const char *path, const char *out) {
    codegen_out = fopen(out, "w");
    if (codegen_out == NULL) {
        fprintf(stdout, "error: failed to open file %s.\n", out);
        return -1;
    }

    yyin = fopen(path, "r");
    if (yyin == NULL) {
        fprintf(stdout, "error: failed to open file %s.\n", path);
        return -1;
    }

    // Perform parsing.
    if (yyparse() != 0)
        return -1;
    fclose(yyin);

    // Perform name resolution.
    scope_enter();
    decl_resolve(ast);
    scope_exit();
    if (resolve_errors > 0)
        return -1;

    // Perform typechecking.
    decl_typecheck(ast);
    if (type_errors > 0)
        return -1;

    // Generate assembly code.
    decl_codegen(ast);

    return 0;
}
