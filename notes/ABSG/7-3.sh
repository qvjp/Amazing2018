#!/bin/bash
# 算数测试

# (( ... )) 结构可用来计算并测试算数表达式的结果
# 退出状态将会与[ ... ]结构完全相反

(( 0 ))
echo "Exit status of \"(( 0 ))\" is $?"     # 1

(( 1 ))
echo "Exit status of \"(( 1 ))\" is $?"     # 0

(( 5 > 4 ))
echo "Exit status of \"(( 5 > 4 ))\" is $?"     # 0

(( 5 > 9 ))
echo "Exit status of \"(( 5 > 9 ))\" is $?"     # 1

(( 5 - 5 ))
echo "Exit status of \"(( 5 - 5 ))\" is $?"     # 1

(( 5 / 4 ))
echo "Exit status of \"(( 5 / 4 ))\" is $?"     # 0

(( 1 / 0)) 2>/dev/null
echo "Exit status of \"(( 1 / 0 ))\" is $?"     # 1

exit 0
