#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "options.h"

int encode_file(const char *path);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "error: invalid arguments, expecting option and filename.\n");
        return EXIT_FAILURE;
    }

    if (!strcmp(argv[1], "--encode"))
        return encode_file(argv[2]) == -1 ? EXIT_FAILURE : EXIT_SUCCESS;
    else if (!strcmp(argv[1], "--scan"))
        return scan_file(argv[2]) == -1 ? EXIT_FAILURE : EXIT_SUCCESS;
    else {
        fprintf(stderr, "error: unknown option \"%s\".\n", argv[1]);
        return EXIT_FAILURE;
    }
}
