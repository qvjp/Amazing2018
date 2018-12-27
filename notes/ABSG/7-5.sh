#!/bin/bash
# 算数比较与字符串比较

a=4
b=5

echo

if [ "$a" -ne "$b" ]
then
    echo "$a is not equal to $b"
    echo "(arithmetic comparison)"
fi

echo

if [ "$a" != "$b" ]
then
    echo "$a is not equal to $b"
    echo "(string comparison)"
fi
# 在这个特定的例子里，-ne和!=都可以

echo
exit 0
