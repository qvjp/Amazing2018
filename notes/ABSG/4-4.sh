#!/bin/bash
# int-or-string

a=2334          # 整形
let "a += 1"
echo "a = $a"   # a = 2335
echo

b=${a/23/BB}    # 将”23“替换成”BB“
echo "b = $b"
declare -i b    # declare也不会对此有任何帮助
echo "b = $b"   # b = BB35

let "b += 1"
echo "b = $b"   # b = 1
echo

c=BB34
echo "c = $c"
d=${c/BB/23}    # 将“BB”替换为“23”，这将使d变为一个整形

echo "d = $d"   # d = 2334
let "d += 1"
echo "d = $d"   # d = 2335
echo

# null变量会如何呢
e=""
echo "e = $e"
let "e += 1"    # 算数操作使null转变为整形
echo "e = $e"
echo

# 如果没有声明变量会怎样
echo "f = $f"
let "f += 1"
echo "f = $f"   # 未声明的变量将被转换成整型变量
echo

# 所以说bash的变量都是不区分类型的

exit 0
