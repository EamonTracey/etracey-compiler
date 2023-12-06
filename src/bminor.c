#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stdout, "error: not enough arguments, expecting option and filename(s).\n");
        return EXIT_FAILURE;
    }

    if (argc == 3) {
        if (!strcmp(argv[1], "--encode"))
            return encode_file(argv[2]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        else if (!strcmp(argv[1], "--scan"))
            return scan_file(argv[2]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        else if (!strcmp(argv[1], "--parse"))
            return parse_file(argv[2]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        else if (!strcmp(argv[1], "--print"))
            return print_file(argv[2]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        else if (!strcmp(argv[1], "--resolve"))
            return resolve_file(argv[2]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
        else if (!strcmp(argv[1], "--typecheck"))
            return typecheck_file(argv[2]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    } else if (argc == 4) {
        if (!strcmp(argv[1], "--codegen"))
            return codegen_file(argv[2], argv[3]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    } else {
        fprintf(stdout, "error: too many arguments.\n");
    }
    
    fprintf(stdout, "error: unknown option \"%s\" with %d arguments.\n", argv[1], argc);
    return EXIT_FAILURE;
}
