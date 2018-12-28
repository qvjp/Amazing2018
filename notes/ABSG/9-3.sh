#!/bin/bash
# 定时输入

TIMELIMIT=3

PrintAnswer()
{
    if [ "$answer" = TIMEOUT ]
    then
        echo $answer
    else
        echo "You favorite veggle is $answer."
        kill $!     # $!是上一个在后台运行作业的PID
    fi
}

TimerOn()
{
    sleep $TIMELIMIT && kill -s 14 $$ &
}

Int14Vector()
{
    answer="TIMEOUT"
    PrintAnswer
    exit 14
}

trap Int14Vector 14     # 定时中断(14)会暗中给定时间限制

echo "What is you favorite vegetable."
TimerOn
read answer
PrintAnswer

