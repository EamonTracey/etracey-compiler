CC=gcc
CFLAGS=-Wall -Werror -Iinclude -Wno-unused-function

bminor: src/*.c src/scanner.c
	$(CC) $(CFLAGS) $^ -o bminor

src/scanner.c: src/scanner.flex
	flex -osrc/scanner.c $^

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
	rm -f test/encoder/*.out
	rm -f test/scanner/*.out
