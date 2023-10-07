#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "error.h"
#include "parser.h"
#include "scanner.h"

extern int yyparse();

int encode_file(const char *path) {
    char line[2048];

    FILE *stream = fopen(path, "r");
    if (stream == NULL) {
        fprintf(stderr, "error: failed to open file \"%s\".\n", path);
        return -1;
    }

    if (fgets(line, 2048, stream) == NULL) {
        fprintf(stderr, "error: failed to read file \"%s\".\n", path);
        fclose(stream);
        return -1;
    }
    fclose(stream);

    line[strcspn(line, "\n")] = '\0';
    
    char dec[256];
    int ret;
    if ((ret = string_decode(line, dec)) != 0) {
        fprintf(stderr, "encode error: %s\n", strencerr[ret]);
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
        fprintf(stderr, "error: failed to open file \"%s\".\n", path);
        return -1;
    }

    int ret;
    char errmsg[2048];

    long int bmint;
    double bmfloat;
    char bmchar;
    char bmstring[2048];

    int token;
    while ((token = yylex()) > 0) {
        switch (token) {
        case TOKEN_INTEGER_LITERAL:
            if ((ret = integer_decode(yytext, &bmint)) == 0)
                fprintf(stdout, "%s %ld\n", tokstr[token - 258], bmint);
            else {
                snprintf(errmsg, 2048, intencerr[ret], yytext);
                fprintf(stderr, "scan error: %s\n", errmsg);
                return -1;
            }
            break;
        case TOKEN_FLOAT_LITERAL:
            if ((ret = float_decode(yytext, &bmfloat)) == 0)
                fprintf(stdout, "%s %lf\n", tokstr[token - 258], bmfloat);
            else {
                snprintf(errmsg, 2048, floatencerr[ret], yytext);
                fprintf(stderr, "scan error: %s\n", errmsg);
                return -1;
            }
            break;
        case TOKEN_CHAR_LITERAL:
            if ((ret = char_decode(yytext, &bmchar)) == 0)
                fprintf(stdout, "%s %c\n", tokstr[token - 258], bmchar);
            else {
                fprintf(stderr, "scan error: %s\n", charencerr[ret]);
                return -1;
            }
            break;            
        case TOKEN_STRING_LITERAL:
            if ((ret = string_decode(yytext, bmstring)) == 0)
                fprintf(stdout, "%s %s\n", tokstr[token - 258], bmstring);
            else {
                fprintf(stderr, "scan error: %s\n", strencerr[ret]);
                return -1;
            }
            break;
        case TOKEN_IDENTIFIER:
            fprintf(stdout, "%s %s\n", tokstr[token - 258], yytext);
            break;
        case TOKEN_IDENTL:
            fprintf(stderr, "scan error: %s\n", scanerr[ERROR_SCAN_IDENTL]);
            return -1;
        case TOKEN_INVALID:
            snprintf(errmsg, 2048, scanerr[ERROR_SCAN_INVALID], yytext);
            fprintf(stderr, "scan error: %s\n", errmsg);
            return -1;
        case TOKEN_IDENTN:
            fprintf(stderr, "scan error: %s\n", scanerr[ERROR_SCAN_IDENTN]);
            return -1;
        case TOKEN_UNTCOM:
            fprintf(stderr, "scan error: %s\n", scanerr[ERROR_SCAN_UNTCOM]);
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
        fprintf(stderr, "error: failed to open file \"%s\".\n", path);
        return -1;
    }

    if (yyparse() != 0) {
        fprintf(stderr, "Parse failed.\n");
        return 1;
    }

    fprintf(stdout, "Parse successful!\n");
    fclose(yyin);

    return 0;
}
