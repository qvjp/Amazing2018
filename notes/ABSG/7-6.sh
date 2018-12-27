#!/bin/bash
# 检查字符串是否为null

if [ -n $string1 ]
then
    echo "String \"string1\" is not null."
else
    echo "Stirng \"string1\" is null."
fi
echo

if [ -n "$string1" ] #string被引号括起来
then
    echo "String \"string1\" is not null."
else
    echo "Stirng \"string1\" is null."
fi
echo

if [ $string1 ] #string被引号括起来
then
    echo "String \"string1\" is not null."
else
    echo "Stirng \"string1\" is null."
fi
echo

string1=initialized

if [ -n $string1 ]
then
    echo "String \"string1\" is not null."
else
    echo "Stirng \"string1\" is null."
fi
echo

if [ -n "$string1" ] #string被引号括起来
then
    echo "String \"string1\" is not null."
else
    echo "Stirng \"string1\" is null."
fi
echo

if [ $string1 ] #string被引号括起来
then
    echo "String \"string1\" is not null."
else
    echo "Stirng \"string1\" is null."
fi
echo

string1="a = b"

if [ $string1 ]
then
    echo "String \"string1\" is not null."
else
    echo "Stirng \"string1\" is null."
fi
echo

exit 0
