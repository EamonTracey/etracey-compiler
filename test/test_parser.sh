#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
NOCOLOR="\e[0m"

echo "Testing good parser cases..."
for testfile in $(ls test/parser/good*.bminor)
do
	if ./bminor --parse $testfile > $testfile.out 2>&1
	then
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	else
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	fi
done

echo "Testing bad parser cases..."
for testfile in $(ls test/parser/bad*.bminor)
do
	if ./bminor --parse $testfile > $testfile.out 2>&1
	then
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	else
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	fi
done
