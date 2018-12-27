#!/bin/bash

a=23        # 简单的赋值
echo $a
b=$a
echo $b

# 现在让我们来点小变化（命令替换）

a=`echo Hello!`     # 把'echo'命令的结果传给变量'a'
echo $a
# 注意，如果在一个命令替换结果中包含一个'!'的话，
# 那么在命令行下将无法工作。
# 因为这触发了bash的”历史机制“
# 但是，在脚本中使用的话，历史功能是被禁止的，所以就能正常运行。

a=`ls -l`
echo $a     # 没有引号包含会删除ls结果中多余的tab和换行符
echo
echo "$a"   # 会保留结果中的空白

R=$(cat /etc/redhat-release)
arch=$(uname -m)
echo $R
echo
echo $arch

exit 0
