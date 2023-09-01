CC=gcc
CFLAGS=-Wall -Werror -Iinclude

bminor: src/bminor.c src/encode.c
	$(CC) $(CFLAGS) $^ -o bminor

.PHONY: test
test: bminor test/test_all.sh
	test/test_all.sh

.PHONY: clean
clean:
	rm -f test/encode/*.out
	rm -f bminor
