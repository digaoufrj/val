#!/bin/sh

# Cada variavel diz se aquele campo deve ser impresso (1) ou nao (0)
s=0; n=0; r=0; v=0; m=0; p=0; i=0; o=0

# --help e --version sao opcoes longas, que o getopts nao entende.
# Por isso sao tratadas antes, olhando o primeiro argumento.
case "$1" in
    --help)
        cat <<'FIM'
Usage: uname [OPTION]...
Print certain system information.  With no OPTION, same as -s.

  -a, --all                print all information, in the following order,
                             except omit -p and -i if unknown:
  -s, --kernel-name        print the kernel name
  -n, --nodename           print the network node hostname
  -r, --kernel-release     print the kernel release
  -v, --kernel-version     print the kernel version
  -m, --machine            print the machine hardware name
  -p, --processor          print the processor type (non-portable)
  -i, --hardware-platform  print the hardware platform (non-portable)
  -o, --operating-system   print the operating system
      --help        display this help and exit
      --version     output version information and exit

GNU coreutils online help: <https://www.gnu.org/software/coreutils/>
Full documentation <https://www.gnu.org/software/coreutils/uname>
or available locally via: info '(coreutils) uname invocation'
FIM
        exit 0
        ;;
    --version)
        cat <<'FIM'
uname (GNU coreutils) 9.4
Copyright (C) 2023 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

Written by David MacKenzie.
FIM
        exit 0
        ;;
esac

# O getopts le as opcoes uma por uma, inclusive juntas (ex.: -sr).
# O ":" no inicio faz o getopts deixar a mensagem de erro por nossa conta.
while getopts ":asnrvmpio" opcao; do
    case "$opcao" in
        a) s=1; n=1; r=1; v=1; m=1; p=1; i=1; o=1 ;;
        s) s=1 ;;
        n) n=1 ;;
        r) r=1 ;;
        v) v=1 ;;
        m) m=1 ;;
        p) p=1 ;;
        i) i=1 ;;
        o) o=1 ;;
        *)
            echo "uname: invalid option -- '$OPTARG'" >&2
            echo "Try 'uname --help' for more information." >&2
            exit 1
            ;;
    esac
done

# Descarta as opcoes ja lidas. Se sobrar algo, e um argumento que o uname
# nao aceita (ex.: "uname foo")
shift $((OPTIND - 1))
if [ "$#" -gt 0 ]; then
    echo "uname: extra operand ‘$1’" >&2
    echo "Try 'uname --help' for more information." >&2
    exit 1
fi

# Sem nenhuma opcao, o uname funciona como -s
if [ "$s$n$r$v$m$p$i$o" = "00000000" ]; then
    s=1
fi

maquina=$(LC_ALL=C lscpu | awk '/^Architecture:/ {print $2}')

# Monta tudo numa linha so, sempre nesta ordem (igual ao uname original)
saida=""
if [ $s -eq 1 ]; then saida="$saida $(cat /proc/sys/kernel/ostype)"; fi
if [ $n -eq 1 ]; then saida="$saida $(hostname)"; fi
if [ $r -eq 1 ]; then saida="$saida $(cat /proc/sys/kernel/osrelease)"; fi
if [ $v -eq 1 ]; then saida="$saida $(cat /proc/sys/kernel/version)"; fi
if [ $m -eq 1 ]; then saida="$saida $maquina"; fi
if [ $p -eq 1 ]; then saida="$saida $maquina"; fi
if [ $i -eq 1 ]; then saida="$saida $maquina"; fi
if [ $o -eq 1 ]; then saida="$saida GNU/Linux"; fi

# Imprime sem o espaco que sobrou no comeco
echo "${saida# }"
