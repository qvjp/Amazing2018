#!/bin/bash
# 测试$*和$@

E_BADARGS=65

if [ ! -n "$1" ]
then
    echo "Usage: `basename $0` argument1 argument2 etc."
    exit $E_BADARGS
fi

echo

index=1

echo "Listing args with \"\$*\":"

for arg in "$*"
do
    echo "Arg #$index = $arg"
    let "index+=1"
done
echo "Entire arg list seen as sigle world"

echo 

index=1

echo "Listing args with \"\$@\":"

for arg in "$@"
do
    echo "Arg #$index = $arg"
    let "index+=1"
done
echo "Entire arg list seen as separate world"

echo 


index=1

echo "Listing args with \$* (unquoted):"

for arg in $*
do
    echo "Arg #$index = $arg"
    let "index+=1"
done
echo "Entire arg list seen as separate world"

echo 
