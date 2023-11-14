#include <stdarg.h>
#include <stdio.h>

int verb = 0;

void verbose(const char *format, ...) {
    va_list args;
    va_start(args, format);

    /* Not yet implemented. */

    va_end(args);
}
