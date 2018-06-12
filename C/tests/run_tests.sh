#!/bin/sh

# KataDictionaryReplacer by Ismo Suihko, 2017-01-01
#
# See TESTING.txt
#
# Run this to execute the tests
#     sh run_tests.sh
#
# An empty line is printed between each tests

#
# Executable in parent directory
# Modify if needed!
#

date

BIN=../kdr

echo
echo TEST 1
$BIN

echo
echo TEST 2
$BIN sanakirja

# Makefile is in above directory
echo
echo TEST 3a
$BIN ../Makefile

echo
echo TEST 3b
$BIN dict_format.txt

echo
echo TEST 3c
$BIN dict_no_comma.txt

echo
echo TEST 3d
echo '$temp$ name $name$' | $BIN dict_duplicate.txt

#
# For the next tests use parameter -d1 to print also input parameters
# And now use dictionary file dict_empty for a few tests
#
BIN="../kdr -d1 dict_empty.txt"

echo
echo TEST 4a
echo '$temp$ here comes the name $name$' | $BIN

echo
echo TEST 4b
echo | $BIN

echo
echo TEST 4c
echo 'kjdf askZ$$$1412' | $BIN

# Not try with debug option -d3
BIN="../kdr -d3 dict_empty.txt"

echo
echo TEST 4d
echo '$temp$' | $BIN

#
# And now use dict1.txt for the rest of the tests
#
BIN="../kdr -d1 dict1.txt"

echo
echo TEST 5a
echo '$temp$ here comes the name $name$' | $BIN

echo
echo TEST 5b
echo 'And here are many short keys to fill output buffer. $x$ $Y$ 1 $x$ $Y$ 2 $x$ $Y$ 3 $x$ $Y$ 4 $x$ $Y$ 5 Some text.' | $BIN

echo
echo TEST 5c
echo 'Text with $name_of_the_key_is_long$. And $Another long key$. How it went?' | $BIN

echo
echo TEST 5d
echo 'This $house$ is not same as this another $House$.' | $BIN

echo
echo TEST 5e
echo 'Price was $$50$.' | $BIN

echo
echo 'How about this? $price is $50$' | $BIN

echo
echo TEST 5f
echo '$ou$ $ou$to$ works as expected.' | $BIN

echo
echo 'But here keys are not separated $3210$ and $3210$0123$ but it was expected.' | $BIN

echo
echo TEST 5g
echo 'Empty key $$ is expanded. More $$ and $$.' | $BIN

# no trailing newline to simulate pressing CTRL-D in the beginning of line
echo
echo TEST 5h
echo -n '' | $BIN

echo
echo TEST 6
BIN="../kdr -d2 dict1.txt"
echo '$price$' | $BIN

