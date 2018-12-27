#!/bin/bash
# 转义字符

echo;echo

echo "\v\v\v\v"         # 逐字符的打印\v\v\v\v
echo "==============="
# 使用-e选项的echo命令来逐字符打印转义字符
echo "VERTICAL TABS"
echo -e "\v\v\v\v"
echo "==============="

echo "QUOTATION MARK"
echo -e "\042"          # "的ASCII码就是42（八进制）
echo "==============="

# 如果使用$'\X'结果，那-e选项就不必要了。
echo;echo "NEWLINE AND BEEP"
echo $'\n'              # 新行
echo $'\a'              # 蜂鸣
echo "==============="

echo "QUOTATION MARKS"
# 版本2以后的bash允许使用$'\nnn'结构（8进制）
echo $'\t \042 \t'      # 被水平制表符括起来的"
# 也可以使用16进制
echo $'\t \x22 \t'      # 被水平制表符括起来的"
echo "==============="

# 分配ASCI字符到变量
# --------------------------------------------
quote=$'\042'
echo "$quote This is a quoted string, $quote and this lies outside the quotes."
echo

# 变量中的连续ASCI字符
triple_underline=$'\137\137\137'    # 137是八进制的_
echo "$triple_underline UNDERLINE $triple_underline"
echo

ABC=$'\101\102\103\010'             # 101, 102, 103是八进制的ABC
echo $ABC

echo; echo

escape=$'\033'                      # 033是八进制的esc
echo "\"escape\" echoes as $escape"

echo; echo


exit 0
