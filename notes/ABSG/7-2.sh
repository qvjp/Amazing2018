#!/bin/bash
# /usr/bin/test, [], /usr/bin/[都是等价命令

echo

if test -z "$1"
then
    echo "No command-line arguments."
else
    echo "First command-line argument is $1."
fi

echo

if /usr/bin/test -z "$1"
then
    echo "No command-line arguments."
else
    echo "First command-line argument is $1."
fi

echo

if [ -z "$1" ]
    #if [ -z "$1        #报错，缺少]
then
    echo "No command-line arguments."
else
    echo "First command-line argument is $1."
fi

echo

if /usr/bin/[ -z "$1" ]
    #if [ -z "$1        #报错，缺少]
then
    echo "No command-line arguments."
else
    echo "First command-line argument is $1."
fi

echo

dir=/home/junping

if cd "$dir" 2>/dev/null
then
    echo "Now in $dir."
else
    echo "Cant change to $dir."
fi

exit 0
