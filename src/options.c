#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "error.h"
#include "options.h"
#include "scanner.h"
#include "token.h"

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

    long int bmint;
    double bmfloat;
    char bmstring[2048];
    int ret;
    char errmsg[2048];

    int token;
    while ((token = yylex()) > 0) {
        switch (token) {
        case TOKEN_INTEGER_LITERAL:
            if ((ret = integer_decode(yytext, &bmint)) == 0)
                fprintf(stdout, "%s %ld\n", tokstr[token], bmint);
            else {
                snprintf(errmsg, 2048, intencerr[ret], yytext);
                fprintf(stderr, "scan error: %s\n", errmsg);
                return -1;
            }
            break;
        case TOKEN_FLOAT_LITERAL:
            if ((ret = float_decode(yytext, &bmfloat)) == 0)
                fprintf(stdout, "%s %lf\n", tokstr[token], bmfloat);
            else {
                snprintf(errmsg, 2048, floatencerr[ret], yytext);
                fprintf(stderr, "scan error: %s\n", errmsg);
                return -1;
            }
            break;
        case TOKEN_STRING_LITERAL:
            if ((ret = string_decode(yytext, bmstring)) == 0)
                fprintf(stdout, "%s %s\n", tokstr[token], bmstring);
            else {
                fprintf(stderr, "scan error: %s\n", strencerr[ret]);
                return -1;
            }
            break;
        default:
            fprintf(stdout, "%s\n", tokstr[token]);
        }
    }

    fclose(yyin);

    return 0;
}
