#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
NOCOLOR="\e[0m"

echo "Testing good codegen cases..."
for testfile in test/codegen/good*.bminor
do
	if ./bminor --codegen $testfile > $testfile.out 2>&1
	then
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	else
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	fi
done
