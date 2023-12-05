#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
NOCOLOR="\e[0m"

echo "Testing good codegen cases..."
for testfile in test/codegen/good*.bminor
do
	if ./bminor --codegen $testfile .test.s > $testfile.out 2>&1 && gcc .test.s src/library.c -o .test.be > /dev/null 2>&1
	then
        ./.test.be > $testfile.be.out 2>&1
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	else
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	fi

    rm -f .test.s
    rm -f .test.be
done

