#!/bin/sh

echo "Digite o nome do arquivo:"
read arquivo

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
