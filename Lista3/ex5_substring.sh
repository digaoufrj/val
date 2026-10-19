#!/bin/sh

if [ $# -ne 2 ] || [ -z "$1" ]
then
    exit 1
fi

case "$2" in
    *"$1"*)
        echo "$1 está contida em $2"
        ;;
esac

exit 0
