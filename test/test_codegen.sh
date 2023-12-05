#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
NOCOLOR="\e[0m"

echo "Testing good codegen cases..."
for testfile in test/codegen/good*.bminor
do
	if ./bminor --codegen $testfile $testfile.s > $testfile.out 2>&1 && gcc $testfile.s src/library.c -o $testfile.exe > /dev/null 2>&1
	then
        ./$testfile.exe > $testfile.exe.out 2>&1
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	else
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	fi
done

