#!/bin/bash
# 一个Bash脚本的正确开头

# Cleanup，版本2

# 使用root身份运行
# 在此处插入代码，打印错误消息，并在不是root时退出

LOG_DIR=/var/log
# 使用变量比把代码写死好
cd $LOG_DIR

cat /dev/null > system.log
cat /dev/null > wifi.log

echo "Logs cleaned up."

exit # 这个命令是一种正确并合适的退出脚本的方法
