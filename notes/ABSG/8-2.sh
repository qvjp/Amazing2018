#!/bin/bash
# 使用10种不同的方法计数到11

n=1; echo -n "$n "

let "n = $n + 1"
echo -n "$n "

: $(( n = $n + 1 ))     # : 是必须的，因为没有:的话
                        # bash将会尝试把$(( n = $n + 1 ))解释为一个命令
echo -n "$n "

(( n = n + 1))
echo -n "$n "

n=$(($n + 1))
echo -n "$n "

: $[ n = $n + 1 ]
echo -n "$n "

n=$[ $n + 1 ]
echo -n "$n "

let "n++"
echo -n "$n "

(( n++ ))
echo -n "$n "

: $(( n++ ))
echo -n "$n "

: $[ n++ ]
echo -n "$n"


echo
exit 0
