#!/bin/bash
# 我是root吗

ROOT_UID=0

if [ "$UID" -eq "$ROOT_UID" ]
then
    echo "You are root"
else
    echo "You are just an ordinary user(但妈妈同意爱你，哈哈哈哈，海王一家既视感)"
fi

exit 0

# 另一种方法

ROOTUSER_NAME=root

username=`id -nu`
# or
username=`whoami`
#if
#then
#else
#fi
