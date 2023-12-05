CC=gcc
CFLAGS=-Wall -Werror -Iinclude -Wno-unused-function

bminor: src/*.c src/scanner.c src/parser.c
	$(CC) $(CFLAGS) $^ -o bminor

src/scanner.c: src/scanner.flex
	flex -osrc/scanner.c src/scanner.flex

src/parser.c: src/parser.bison
	bison --defines=include/parser.h --output=src/parser.c -v src/parser.bison

.PHONY: test
test: bminor test/test_all.sh
	test/test_all.sh

.PHONY: test-encoder
test-encoder: bminor test/test_encoder.sh
	test/test_encoder.sh

.PHONY: test-scanner
test-scanner: bminor test/test_scanner.sh
	test/test_scanner.sh

.PHONY: clean
clean:
	rm -f bminor
	rm -f src/scanner.c
	rm -f src/parser.c
	rm -f src/parser.output
	rm -f include/parser.h
	rm -f test/encode/*.out*
	rm -f test/scanner/*.out*
	rm -f test/parser/*.out*
	rm -f test/printer/*.out*
	rm -f test/resolver/*.out*
	rm -f test/typechecker/*.out*
	rm -f test/codegen/*.out*
	rm -f .test.s
	rm -f .test.be
