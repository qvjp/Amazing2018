#!/bin/bash
# $IFS 处理空白与其他字符不同

output_args_one_per_line()
{
    for arg
    do
        echo "[$arg]"
    done
}

echo; echo "IFS=\" \""
echo "------"

IFS=" "
var="a  b c   "
output_args_one_per_line $var

echo; echo "IFS=:"
echo "------"

IFS=:
var=":a::b:c:::"
output_args_one_per_line $var

echo
exit 0
