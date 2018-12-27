#!/bin/bash
# echo的一些诡异特性

var="'(]\\{}\$\""
echo $var
echo "$var"         # 这两个都是'(]\{}$"

echo

IFS='\'
echo $var           # \被空白替换了
echo "$var"         # '(]\{}$"

exit 0
