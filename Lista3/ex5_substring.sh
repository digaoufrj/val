#!/bin/sh

if [ $# -ne 2 ]
then
    exit 1
fi

if echo "$2" | grep "$1" > /dev/null
then
    echo "$1 está contida em $2"
fi

exit 0
