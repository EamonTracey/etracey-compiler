#!/bin/bash

GREEN="\e[32m"
RED="\e[31m"
NOCOLOR="\e[0m"

# For the printer, there are no 'bad' test cases.
# Rather, ensure printing and reprinting yield
# the exact same output.
echo "Testing good printer cases..."
for testfile in $(ls test/printer/good*.bminor)
do
    ./bminor --print $testfile > $testfile.out1 2>&1
    ./bminor --print $testfile.out1 > $testfile.out2 2>&1
	if cmp $testfile.out1 $testfile.out2
	then
        echo -e "${GREEN}PASS${NOCOLOR}: $testfile"
	else
        echo -e "${RED}FAIL${NOCOLOR}: $testfile"
	fi
done
