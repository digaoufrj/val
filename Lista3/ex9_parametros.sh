#!/bin/sh

i=1
for p in "$@"
do
    echo "Parâmetro $i: $p"
    i=$(($i+1))
done

exit 0
