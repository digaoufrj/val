#!/bin/sh

if [ $# -ne 1 ]
then
    echo "Uso: $0 numero"
    exit 1
fi

if ! [ "$1" -eq "$1" ] 2> /dev/null
then
    echo "Erro: '$1' não é um número inteiro"
    exit 1
fi

if [ "$1" -lt 0 ]
then
    echo "Erro: o número precisa ser maior ou igual a zero"
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
