#!/bin/sh

if [ $# -ne 1 ]
then
    echo "Uso: $0 numero"
    exit 1
fi

n=$1
while [ "$n" -ge 0 ]
do
    echo -n "$n "
    n=$(($n-1))
done
echo

exit 0
