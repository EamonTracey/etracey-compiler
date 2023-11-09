#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
NOCOLOR="\e[0m"

echo "Testing good resolver cases..."
for testfile in test/resolver/good*.bminor
do
	if ./bminor --resolve $testfile > $testfile.out 2>&1
	then
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	else
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	fi
done

echo "Testing bad resolver cases..."
for testfile in test/resolver/bad*.bminor
do
	if ./bminor --resolve $testfile > $testfile.out 2>&1
	then
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	else
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	fi
done
