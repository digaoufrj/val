#!/bin/sh

if [ "$#" -eq 0 ]; then
    cat /proc/sys/kernel/ostype
    exit 0
fi

while [ "$#" -gt 0 ]; do
    case "$1" in
        '-s')
            cat /proc/sys/kernel/ostype
            ;;
        '-r')
            cat /proc/sys/kernel/osrelease
            ;;
        '-v')
            cat /proc/sys/kernel/version
            ;;
        '-m')
		echo $(LC_ALL=C lscpu | awk '/^Architecture:/ {print $2}')
            ;;
        '-n')
            hostname
            ;;
        '-o')
            echo 'GNU/Linux'
            ;;
        *)
            echo "new_uname: opção não existe -- '$1'" >&2
            exit 1
            ;;
    esac
    
    shift
done