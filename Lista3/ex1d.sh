#!/bin/sh

# Mostra informacoes do sistema: horario, ocupacao do disco
# e usuarios logados. Pede confirmacao antes de comecar.

# Pergunta ao usuario se ele quer continuar
echo "Deseja ver as informacoes do sistema? (s/n)"
read resposta

# Se a resposta nao for "sim", o script termina aqui
case "$resposta" in
    [sS] | [sS][iI][mM])
        echo
        ;;
    *)
        echo "Ok, nada sera mostrado."
        exit 0
        ;;
esac

# Data e horario
echo "=============================================="
echo " Data e horario (comando date)"
echo "=============================================="
date
echo

# Ocupacao do disco (-h mostra os tamanhos em K, M, G)
echo "=============================================="
echo " Ocupacao do disco (comando df)"
echo "=============================================="
df -h
echo

# Usuarios logados no sistema
echo "=============================================="
echo " Usuarios logados (comando who)"
echo "=============================================="
who

exit 0
