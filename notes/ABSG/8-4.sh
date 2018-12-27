#!/bin/bash

# 几种不同数制的数字表示方法

# 10进制：默认
let "dec = 32"
echo "decimal number = $dec"

let "oct = 032"
echo "octal number = $oct"      # 结果以10进制表示

let "hex = 0x32"
echo "hexadecimal number = $hex"

# 其他进制BASE#NUMBER
# BASE的范围2到64
let "bin = 2#111100111001101"
echo "binary number = $bin"

let "b32 = 32#77"
echo "base-32 number = $b32"

let "b64 = 64#@_"
echo "base-64 number = $b64"
# 这个表示方法只能工作于受限的ASCII字符范围
# 10个数字+26个小写字母+26个大写字母+@+_

echo

echo $((36#zz)) $((2#10101010)) $((16#AF16)) $((53#laA))

# 如果一个超出给定进制的数字的话，将会引起一个错误
let "bad_oct = 081"

exit 0

