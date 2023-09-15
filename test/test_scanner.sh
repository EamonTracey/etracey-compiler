#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
NOCOLOR="\e[0m"

echo "Testing good scanner cases..."
for testfile in test/scanner/good*.bminor
do
	if ./bminor --scan $testfile > $testfile.out 2>&1
	then
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	else
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	fi
done

echo "Testing bad scanner cases..."
for testfile in test/scanner/bad*.bminor
do
	if ./bminor --scan $testfile > $testfile.out 2>&1
	then
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	else
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	fi
done
