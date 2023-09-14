#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "options.h"
#include "scanner.h"
#include "token.h"

const char *token_strs[] = {
    "",
    "ARRAY",
    "AUTO",
    "BOOLEAN",
    "CHAR",
    "ELSE",
    "FALSE",
    "FLOAT",
    "FOR",
    "FUNCTION",
    "IF",
    "INTEGER",
    "PRINT",
    "RETURN",
    "STRING",
    "TRUE",
    "VOID",
    "WHILE",
    "INCREMENT",
    "DECREMENT",
    "NOT",
    "EXPONENTIATION",
    "MULTIPLICATION",
    "DIVISION",
    "MODULO",
    "ADDITION",
    "SUBTRACTION",
    "LESSER",
    "LESSER_EQUAL",
    "GREATER",
    "GREATER_EQUAL",
    "EQUALITY",
    "INEQUALITY",
    "AND",
    "OR",
    "ASSIGNMENT",
    "BRACE_OPEN",
    "BRACE_CLOSE",
    "PAREN_OPEN",
    "PAREN_CLOSE",
    "BRACK_OPEN",
    "BRACK_CLOSE",
    "COLON",
    "SEMICOLON",
    "COMMA",
    "COMMENT_SINGLE",
    "COMMENT_MULTI",
    "INTEGER_LITERAL",
    "FLOAT_LITERAL",
    "STRING_LITERAL",
    "CHAR_LITERAL",
    "IDENTIFIER"
};
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
    switch (ret = string_decode(line, dec)) {
        case 0:
            break;
        case ENC_BQUOTE:
            fprintf(stderr, "error: string literal must begin with quotation mark.\n");
            return -1;
        case ENC_PRINTABLE:
            fprintf(stderr, "error: string literal must contain only printable characters and backslash codes.\n");
            return -1;
        case ENC_LENGTH:
            fprintf(stderr, "error: string must be at most 255 characters.\n");
            return -1;
        case ENC_TRAIL:
            fprintf(stderr, "error: string literal must end with quotation mark.\n");
            return -1;
        case ENC_HEX:
            fprintf(stderr, "error: hex backslash code must be of valid form \\0xHH.\n");
            return -1;
        case ENC_CODE:
            fprintf(stderr, "error: invalid backslash code, use only a, b, e, f, n, r, t, v, \\, ', \", 0xHH.\n");
            return -1;
    }

    // Re-encode the decoded string.
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

    int token;
    while ((token = yylex()) > 0) {
        fprintf(stdout, "%s %d\n", token_strs[token], token);
    }

    return 0;
}
