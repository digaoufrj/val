#!/bin/sh

echo "Mostrando a data e o horario (comando date):"
date
echo

echo "Mostrando a ocupacao do disco (comando df):"
df -h
echo

echo "Mostrando os usuarios logados (comando who):"
who

exit 0
