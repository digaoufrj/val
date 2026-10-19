#!/bin/sh

echo "Digite o nome do arquivo:"
read arquivo

if [ -z "$arquivo" ]
then
    echo "Nenhum nome foi digitado"
    exit 1
fi

if [ -e "$arquivo" ]
then
    if [ -d "$arquivo" ]
    then
        echo "$arquivo existe e é um diretório"
    else
        echo "$arquivo existe e é um arquivo"
    fi
else
    echo "$arquivo não existe"
fi

exit 0
