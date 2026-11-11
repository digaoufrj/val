# Relatório — Trabalho 3: o comando `uname` em Shell Script

**Disciplina:** Sistemas Operacionais 2 / Tópicos Especiais em Ciência da Computação 1 — UFRJ, Instituto de Computação

**Integrantes:** _(preencher)_

---

## 1. Introdução

Neste trabalho, reimplementamos em Shell Script o utilitário `uname`, do Linux.
O objetivo é que o nosso script se comporte exatamente como o comando original:
mesmas saídas, mesmas mensagens de erro e mesmos códigos de saída.

O `uname` (de *Unix name*) mostra informações que identificam o sistema: o nome
do kernel, o nome da máquina na rede, a versão do kernel, a arquitetura do
processador e o sistema operacional. Ele é útil, por exemplo, em scripts de
instalação que precisam saber se a máquina é de 64 bits, ou quando se quer
descobrir qual versão do kernel está rodando.

Como referência, usamos o `uname` do pacote GNU coreutils 9.4, que vem no
Ubuntu 24.04.

## 2. Sintaxe

```
uname [OPÇÃO]...
```

O `uname` só recebe opções. Qualquer outro argumento é tratado como erro.

## 3. As opções

| Opção | Forma longa | O que mostra | Exemplo (na nossa máquina) |
|---|---|---|---|
| `-s` | `--kernel-name` | nome do kernel | `Linux` |
| `-n` | `--nodename` | nome da máquina na rede (*hostname*) | `rodrigo-MS-7D43` |
| `-r` | `--kernel-release` | versão (*release*) do kernel | `7.0.0-38-generic` |
| `-v` | `--kernel-version` | dados da compilação do kernel | `#38~24.04.4-Ubuntu SMP PREEMPT_DYNAMIC Mon Sep 14 16:37:11 UTC 2` |
| `-m` | `--machine` | arquitetura do hardware | `x86_64` |
| `-p` | `--processor` | tipo do processador | `x86_64` |
| `-i` | `--hardware-platform` | plataforma de hardware | `x86_64` |
| `-o` | `--operating-system` | sistema operacional | `GNU/Linux` |
| `-a` | `--all` | todas as informações acima, em uma linha | ver abaixo |
| | `--help` | mostra a ajuda e sai | |
| | `--version` | mostra a versão do programa e sai | |

Exemplo com `-a`:

```
$ uname -a
Linux rodrigo-MS-7D43 7.0.0-38-generic #38~24.04.4-Ubuntu SMP PREEMPT_DYNAMIC Mon Sep 14 16:37:11 UTC 2 x86_64 x86_64 x86_64 GNU/Linux
```

Observações sobre algumas opções:

- **`-r` e `-v` confundem pelo nome.** O `-r` é o número da versão do kernel. O
  `-v` descreve como esse kernel foi compilado: o número da compilação (`#38`),
  características como `SMP` (suporte a vários processadores) e
  `PREEMPT_DYNAMIC` (tipo de preempção do escalonador), e a data.
- **Por que o `-v` termina em "UTC 2"?** O kernel guarda cada campo em um
  vetor de 65 bytes (64 caracteres mais o `\0` que termina a string). Este texto
  tem mais de 64 caracteres, então o final, o ano, foi cortado.
- **`-p` e `-i` não são portáveis** (o próprio `--help` avisa isso). No
  coreutils original eles mostram `unknown` e o `-a` os omite. O Ubuntu aplica
  uma modificação que faz os dois mostrarem a arquitetura da máquina. Por isso o
  `x86_64` aparece três vezes no `-a` (de `-m`, `-p` e `-i`).
- **`-o` é um texto fixo.** No Linux com GNU coreutils é sempre `GNU/Linux`.

## 4. Regras de comportamento

Além do que cada opção mostra, o `uname` segue algumas regras. O nosso script
precisa respeitar todas elas para se comportar igual ao original.

1. **Sem opção nenhuma, funciona como `-s`.**
   ```
   $ uname
   Linux
   ```
2. **Tudo sai em uma linha só**, com os campos separados por espaço.
3. **A ordem é sempre a mesma** (s, n, r, v, m, p, i, o), não importa a ordem
   em que as opções foram digitadas.
   ```
   $ uname -m -s
   Linux x86_64
   ```
4. **Opção repetida aparece uma vez só.** `uname -s -s` mostra `Linux` uma vez.
5. **Opções podem ser juntadas.** `uname -sr` é o mesmo que `uname -s -r`.
6. **`--` marca o fim das opções.** Sozinho não muda nada (`uname --` mostra
   `Linux`).
7. **Opção inválida é erro.** A mensagem vai para a saída de erro (*stderr*) e
   o código de saída é 1:
   ```
   $ uname -x
   uname: invalid option -- 'x'
   Try 'uname --help' for more information.
   ```
8. **Argumento que não é opção também é erro**, porque o `uname` não recebe
   argumentos:
   ```
   $ uname foo
   uname: extra operand ‘foo’
   Try 'uname --help' for more information.
   ```
   Detalhe: nesta mensagem as aspas são curvas (`‘foo’`). Na mensagem de opção
   inválida elas são retas (`'x'`).
9. **Tudo é validado antes de imprimir.** Em `uname -s -x`, o `-s` é válido,
   mas nada é impresso: aparece só a mensagem de erro.
10. **Quando dá certo, o código de saída é 0.**

## 5. De onde vêm as informações

O `uname` original usa a **chamada de sistema `uname()`**. Ela pede ao kernel
uma estrutura (`struct utsname`) já preenchida com o nome do sistema, o nome da
máquina, o *release*, a versão e a arquitetura.

O kernel também mostra essas informações em arquivos dentro de
`/proc/sys/kernel/`. O `/proc` é um **sistema de arquivos virtual**: esses
arquivos não existem no disco. O kernel gera o conteúdo na hora em que alguém
os lê. Assim, um `cat /proc/sys/kernel/osrelease` devolve o mesmo valor que o
`uname -r`.

O nosso script obviamente não chama o `uname`. Ele busca cada informação assim:

| Opção | No `uname` original (`struct utsname`) | No nosso script |
|---|---|---|
| `-s` | campo `sysname` | `cat /proc/sys/kernel/ostype` |
| `-n` | campo `nodename` | comando `hostname` |
| `-r` | campo `release` | `cat /proc/sys/kernel/osrelease` |
| `-v` | campo `version` | `cat /proc/sys/kernel/version` |
| `-m`, `-p`, `-i` | campo `machine` | linha `Architecture` do comando `lscpu` |
| `-o` | texto fixo no programa | texto fixo `GNU/Linux` |

## 6. A nossa implementação

**Arquivos:**

- `new_uname_corrigido.sh` — versão final, descrita neste relatório.
- `new_uname.sh` — primeira versão, antes das correções.

**Como executar:**

```
chmod +x new_uname_corrigido.sh
./new_uname_corrigido.sh -a
```

O script funciona em seis passos.

**Passo 1 — uma variável por campo.** Cada variável diz se aquele campo vai ser
impresso (1) ou não (0). No começo, todas valem 0.

```sh
s=0; n=0; r=0; v=0; m=0; p=0; i=0; o=0
```

**Passo 2 — `--help` e `--version`.** São opções longas, que o `getopts` (usado
no passo 3) não entende. Por isso o script olha o primeiro argumento antes. Se
for uma delas, ele imprime o mesmo texto do original com `cat <<'FIM'`, um
*here document*: o `cat` imprime tudo até a linha `FIM`. Depois disso, o
script sai com código 0.

**Passo 3 — ler as opções com `getopts`.** O `getopts` é um comando do próprio
shell que lê as opções uma por uma. Ele já resolve sozinho as opções juntas
(`-sr`) e o `--`. Cada letra só marca a variável correspondente. O `-a` marca
todas.

```sh
while getopts ":asnrvmpio" opcao; do
    case "$opcao" in
        a) s=1; n=1; r=1; v=1; m=1; p=1; i=1; o=1 ;;
        s) s=1 ;;
        ...
        *)
            echo "uname: invalid option -- '$OPTARG'" >&2
            echo "Try 'uname --help' for more information." >&2
            exit 1
            ;;
    esac
done
```

O `:` no início de `":asnrvmpio"` faz o `getopts` não imprimir a mensagem de
erro dele, para que o script imprima a mesma mensagem do `uname`. Nesse caso,
a letra inválida fica na variável `OPTARG`. O `>&2` manda a mensagem para a
saída de erro.

**Passo 4 — recusar argumentos extras.** O `shift $((OPTIND - 1))` descarta as
opções já lidas. Se ainda sobrar algum argumento (`$#` maior que zero), é um
argumento que o `uname` não aceita:

```sh
shift $((OPTIND - 1))
if [ "$#" -gt 0 ]; then
    echo "uname: extra operand ‘$1’" >&2
    echo "Try 'uname --help' for more information." >&2
    exit 1
fi
```

**Passo 5 — padrão `-s`.** Se nenhuma variável foi marcada, o script marca a
`s`.

**Passo 6 — montar e imprimir a linha.** O script monta a saída na ordem fixa,
colocando um espaço antes de cada campo. No final, `${saida# }` tira o espaço
que sobra no começo.

```sh
saida=""
if [ $s -eq 1 ]; then saida="$saida $(cat /proc/sys/kernel/ostype)"; fi
if [ $n -eq 1 ]; then saida="$saida $(hostname)"; fi
...
echo "${saida# }"
```

**Por que esse formato garante as regras da seção 4:** as opções só marcam
variáveis e a impressão acontece uma única vez, no final. Por isso:

- a ordem da saída é sempre a mesma;
- uma opção repetida só marca 1 de novo, sem imprimir duas vezes;
- qualquer erro encerra o script antes de imprimir alguma coisa.

## 7. Testes

Comparamos o nosso script com o `uname` original em **38 combinações de
argumentos**, verificando a saída normal, as mensagens de erro e o código de
saída. **Todas deram idênticas.** As combinações cobriram:

- cada opção sozinha e o `-a`;
- opções juntas (`-sr`, `-snrvmpio`), fora de ordem (`-r -s`) e repetidas
  (`-s -s`);
- opções inválidas (`-x`, `-sx`, `-s -x`, `-h`);
- argumentos extras (`foo`, `-s foo`, `-- -s`, `-`);
- `--`, `--help` e `--version`.

Este é um trecho do teste, que pode ser rodado na mesma pasta do script:

```sh
for args in "" "-a" "-sr" "-r -s" "-s -s" "-x" "-s -x" "foo" "--" "--help"; do
    a=$(uname $args 2>&1; echo "saida=$?")
    b=$(./new_uname_corrigido.sh $args 2>&1; echo "saida=$?")
    if [ "$a" = "$b" ]; then
        echo "igual: uname $args"
    else
        echo "DIFERENTE: uname $args"
    fi
done
```

## 8. Limitações

- **Opções longas além de `--help` e `--version` não foram implementadas**
  (`--all`, `--kernel-name` etc.). Para elas, o script responde
  `invalid option -- '-'`. As formas curtas equivalentes (`-a`, `-s` etc.)
  funcionam.
- **`--help` e `--version` só são reconhecidos como primeiro argumento.** O
  original também os aceita depois de outras opções, como em `uname -s --help`.
- **Opção depois de um argumento extra:** em `uname foo -x`, o original reclama
  do `-x`, porque continua lendo opções mesmo depois de um argumento. O nosso
  script reclama do `foo`, porque o `getopts` para de ler no primeiro argumento
  que não é opção.
- **Idioma:** as mensagens estão em inglês, como no `uname` de um sistema
  configurado em inglês (o caso da nossa máquina). Em um sistema com as
  traduções do coreutils para português instaladas, o original responderia em
  português.
- **`-p` e `-i`** seguem o comportamento do Ubuntu (seção 3). Em distribuições
  sem a modificação, o original mostra `unknown`.

## 9. Conclusão

O script implementa nove opções (`-a`, `-s`, `-n`, `-r`, `-v`, `-m`, `-p`,
`-i` e `-o`) mais `--help` e `--version`, acima das cinco pedidas no enunciado.
Ele reproduz as saídas e as mensagens de erro do original.

O trabalho mostrou que o `uname` tem regras que vão além de imprimir
informações, como a ordem fixa dos campos e a validação de todos os argumentos
antes de qualquer saída. Mostrou também que essas informações vêm do kernel,
pela chamada de sistema `uname()` ou pelos arquivos virtuais de
`/proc/sys/kernel/`.

## Referências

- `man 1 uname` — manual do comando.
- `info '(coreutils) uname invocation'` — documentação completa do GNU coreutils.
- `man 2 uname` — a chamada de sistema `uname()` e a `struct utsname`.
- `man 5 proc` — o sistema de arquivos `/proc`.
- `man 1 dash` (seção sobre `getopts`) — leitura de opções em Shell Script.
