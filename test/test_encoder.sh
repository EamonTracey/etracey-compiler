#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
NOCOLOR="\e[0m"

echo "Testing good encoder cases..."
for testfile in test/encode/good*.bminor
do
	if ./bminor --encode $testfile > $testfile.out 2>&1
	then
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	else
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	fi
done

echo "Testing bad encoder cases..."
for testfile in test/encode/bad*.bminor
do
	if ./bminor --encode $testfile > $testfile.out 2>&1
	then
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	else
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	fi
done
