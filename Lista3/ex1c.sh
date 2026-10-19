#!/bin/sh

echo "Deseja ver as informacoes do sistema? (s/n)"
read resposta

case "$resposta" in
    [sS] | [sS][iI][mM])
        echo
        ;;
    *)
        echo "Ok, nada sera mostrado."
        exit 0
        ;;
esac

echo "Mostrando a data e o horario (comando date):"
date
echo

echo "Mostrando a ocupacao do disco (comando df):"
df -h
echo

echo "Mostrando os usuarios logados (comando who):"
who

exit 0
