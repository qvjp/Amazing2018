#!/bin/bash
# 使用‘shift’来逐步存取所有位置的参数

until [ -z "$1" ]
do
    echo -n "$1 "
    shift
done

echo

exit 0
