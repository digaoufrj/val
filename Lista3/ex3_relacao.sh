#!/bin/sh

if [ $# -ne 2 ]
then
    echo "Uso: $0 numero1 numero2"
    exit 1
fi

if ! [ "$1" -eq "$1" ] 2> /dev/null
then
    echo "Erro: '$1' não é um número inteiro"
    exit 1
fi

if ! [ "$2" -eq "$2" ] 2> /dev/null
then
    echo "Erro: '$2' não é um número inteiro"
    exit 1
fi

if [ "$1" -lt "$2" ]
then
    echo "$1 é menor que $2"
elif [ "$1" -gt "$2" ]
then
    echo "$1 é maior que $2"
else
    echo "$1 é igual a $2"
fi

exit 0
