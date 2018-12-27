#!/bin/bash

# 变量赋值和替换

a=375
hello=$a

# ----------------------------------------------------------
# 强烈注意，在赋值的时候，等号前后一定不要有空格。
# 如果出现空格会怎样？
#
# "VARIABLE =value"
#
# 脚本将尝试运行一个"VARIABLE"命令，带着一个"=value"的参数。
#
# "VARIABLE= value"
#
# 脚本将尝试运行一个"value"命令，
# 并且带着一个被赋值成""的环境变量"VARIABLE"。
# ----------------------------------------------------------

echo hello          # 没有变量引用，只是一个hello字符串

echo $hello
echo ${hello}       # 同上
echo "$hello"
echo "${hello}"

echo

hello="A B  C   D"
echo $hello     # A B C D
echo "$hello"   # A B  C   D
# 就像你看到的echo $hello 和echo "$hello" 将给出不同的结果
# ============================================================
# 引用一个变量将保留其中的空白，当然，如果是变量替换就不会保留。
# =============================================================

echo

echo '$hello'   # $hello
# 全引用的作用将会导致"$"被解释为单独的字符，而不是变量前缀。

# 注意这两种引用所产生的不同的效果。

hello=      # 设置为空值
echo "\$hello (null value) = $hello"
# 注意设置一个变量为null，与unset这个变量，并不一样
# 虽然最终结果一样（具体见下边）

# ------------------------------------------------------------

# 可以在同一行设置多个变量
# 但是必须以空白进行分割
# 慎用，这样会降低可读性，并不可移植。

var1=21 var2=22 var3=$V3
echo
echo "var1=$var1 var2=$var2 var3=$var3"

# 在老版本的“sh”上可能会有问题

# -----------------------------------------------------------

echo; echo

numbers="one two three"
other_numbers="1 2 3"
# 如果在变量中存在空白，那么就必须加上引用
# other_number2=1 2 3 会报错
echo "numbers = $numbers"
echo "other_numbers = $other_numbers"

# 不过也可以采用空白转义的方法
mixed_bag=2\ ---\ whatever
echo "$mixed_bag"

echo; echo

echo "uninitialized_variable = $uninitialized_variable"
# Uninitialized变量为nul
uninitialized_variable=     # 声明，但没有初始化这个变量
                            # 其实和前边设置为空值的作用是一样的
echo "uninitialized_variable = $uninitialized_variable"
                            # 还是一个空值

uninitialized_variable=23   # 赋值
echo "uninitialized_variable = $uninitialized_variable"
unset uninitialized_variable # Unset这个变量
echo "uninitialized_variable = $uninitialized_variable"
                            # 还是一个空值
echo

exit 0
