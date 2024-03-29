#ifndef OPTIONS_H
#define OPTIONS_H

int encode_file(const char *);
int scan_file(const char *);
int parse_file(const char *);
int print_file(const char *);
int resolve_file(const char *);
int typecheck_file(const char *);
int codegen_file(const char *, const char *);

#endif
