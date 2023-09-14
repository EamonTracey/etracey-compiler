CC=gcc
CFLAGS=-Wall -Werror -Iinclude -Wno-unused-function

bminor: src/*.c src/scanner.c
	$(CC) $(CFLAGS) $^ -o bminor

src/scanner.c: src/scanner.flex
	flex -osrc/scanner.c $^

.PHONY: test
test: bminor test/test_all.sh
	test/test_all.sh

.PHONY: clean
clean:
	rm -f test/encode/*.out
	rm -f bminor
	rm -f src/scanner.c
