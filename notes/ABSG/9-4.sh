#!/bin/bash
# read定时

TIMELIMIT=4

read -t $TIMELIMIT variable

echo

if [ -z "$variable" ]
then
    echo "Time out, variabe still unset."
else
    echo "variable = $variable"
fi

exit 0
