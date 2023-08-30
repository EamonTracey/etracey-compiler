#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "encode.h"

int encode_file(const char *path);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "error: invalid arguments, expecting option and filename.\n");
        return EXIT_FAILURE;
    }

    if (!strcmp(argv[1], "--encode"))
        return encode_file(argv[2]) == -1 ? EXIT_FAILURE : EXIT_SUCCESS;
    else {
        fprintf(stderr, "error: unknown option \"%s\".\n", argv[1]);
        return EXIT_FAILURE;
    }
}

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
        case ENC_HEXPRINT:
            fprintf(stderr, "error: hex backslash code code must be a printable character.\n");
            return -1;
    }

    // Re-encode the decoded string and print to stdout.
    char reenc[520];
    string_encode(dec, reenc);
    fprintf(stdout, "%s\n", reenc);

    return 0;
}
