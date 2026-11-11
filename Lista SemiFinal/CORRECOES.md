# Do `new_uname.sh` para o `new_uname_corrigido.sh`

Este arquivo explica, uma por uma, as diferenças entre o seu código
([`new_uname.sh`](new_uname.sh)) e o corrigido
([`new_uname_corrigido.sh`](new_uname_corrigido.sh)).

Leia com calma e na ordem, porque cada mudança se apoia na anterior. Se
puder, abra os dois arquivos lado a lado.

---

## Primeiro: o que você já acertou

O seu código acertava **a parte mais difícil de descobrir: de onde vem cada
informação**. Tudo isso continua no corrigido, do mesmo jeito:

- `cat /proc/sys/kernel/ostype` para o `-s`
- `cat /proc/sys/kernel/osrelease` para o `-r`
- `cat /proc/sys/kernel/version` para o `-v`
- `hostname` para o `-n`
- `LC_ALL=C lscpu | awk '/^Architecture:/ {print $2}'` para o `-m`
- o texto fixo `GNU/Linux` para o `-o`
- o erro indo para a saída de erro (`>&2`) e o `exit 1`

Cada opção **sozinha** já funcionava igual ao `uname`.

O que precisou mudar foi **como o script lê as opções** e **quando ele
imprime**. O enunciado pede comportamento idêntico ao do `uname`, inclusive nos
erros. O `uname` tem algumas regras que só aparecem quando se combinam opções
ou quando dá erro.

---

## O mapa: onde foi parar cada pedaço do seu código

| No seu código | No corrigido |
|---|---|
| `if [ "$#" -eq 0 ]` (sem argumento, mostra `Linux`) | virou uma checagem depois da leitura das opções ([linhas 78–80](new_uname_corrigido.sh#L78-L80)) → **Mudança 3** |
| `while [ "$#" -gt 0 ] ... shift ... done` | virou `while getopts ... done` ([linhas 49–66](new_uname_corrigido.sh#L49-L66)) → **Mudança 4** |
| `'-s') cat /proc/sys/kernel/ostype ;;` | dividido em dois: `s) s=1 ;;` no laço, e o `cat` no final ([linha 86](new_uname_corrigido.sh#L86)) → **Mudanças 1 e 2** |
| `echo $(LC_ALL=C lscpu \| awk ...)` | `maquina=$(LC_ALL=C lscpu \| awk ...)` ([linha 82](new_uname_corrigido.sh#L82)) → **Mudança 2** |
| `*) echo "new_uname: opção não existe ..."` | dividido em dois erros: opção inválida ([linhas 60–64](new_uname_corrigido.sh#L60-L64)) e operando extra ([linhas 70–75](new_uname_corrigido.sh#L70-L75)) → **Mudanças 5 e 6** |

---

## Mudança 1 — Anotar em vez de imprimir

Esta é a mudança principal. Entendendo esta, o resto fica fácil.

### O problema

Rode estes comandos no `uname` de verdade e no seu script:

| Comando | `uname` de verdade | Seu script |
|---|---|---|
| `-s -r` | `Linux 7.0.0-38-generic` | `Linux` numa linha e `7.0.0-38-generic` na outra |
| `-r -s` | `Linux 7.0.0-38-generic` | `7.0.0-38-generic` e depois `Linux` |
| `-s -s` | `Linux` | `Linux` duas vezes |
| `-s -x` | só a mensagem de erro | `Linux` e depois a mensagem de erro |

Ou seja, o `uname` de verdade:

1. imprime tudo **numa linha só**;
2. imprime sempre **na mesma ordem** (s, n, r, v, m, p, i, o), não importa a
   ordem em que você digitou;
3. **não repete** uma opção repetida;
4. se tiver **qualquer erro**, não imprime **nada**, só o erro.

### Por que acontece

No seu código, cada opção **imprime na hora** em que é lida:

```sh
'-s')
    cat /proc/sys/kernel/ostype
    ;;
```

Então:

- em `-s -r`, cada `cat` imprime e pula de linha, e saem duas linhas;
- em `-r -s`, o `-r` é lido primeiro, então é impresso primeiro;
- em `-s -s`, o `-s` é lido duas vezes, então é impresso duas vezes;
- em `-s -x`, o `Linux` já foi impresso quando o script chega no `-x` e
  descobre o erro. Aí é tarde demais.

### A correção

Na leitura, o script só **anota** o que foi pedido. Ele imprime tudo **no
final**, de uma vez.

Para anotar, o corrigido cria uma variável para cada campo no começo do script,
todas valendo 0 ([linha 4](new_uname_corrigido.sh#L4)):

```sh
s=0; n=0; r=0; v=0; m=0; p=0; i=0; o=0
```

Pense em cada uma como uma **bandeirinha**: 0 = "não imprimir", 1 = "imprimir".

Dentro do `case`, no lugar do `cat`, só se levanta a bandeirinha:

**Antes (seu código):**

```sh
'-s')
    cat /proc/sys/kernel/ostype
    ;;
'-r')
    cat /proc/sys/kernel/osrelease
    ;;
```

**Depois (corrigido):**

```sh
s) s=1 ;;
r) r=1 ;;
```

(Por que `s)` em vez de `'-s')`? Isso é da Mudança 4. Por enquanto, repare só
que o `cat` saiu daqui.)

O `cat` não sumiu: ele foi para o final do script, que é a Mudança 2.

> **Comparação:** é como fazer compras com uma lista. Você anota tudo
> primeiro e só depois passa pelos corredores, **na ordem em que eles estão no
> mercado**, e não na ordem em que lembrou de cada item. Se anotou "leite"
> duas vezes, pega um só. E se anotou algo que o mercado não vende, você
> descobre isso antes de pegar qualquer coisa.

---

## Mudança 2 — Imprimir tudo no final, numa linha

Depois de ler todas as opções, o corrigido monta a resposta
([linhas 85–96](new_uname_corrigido.sh#L85-L96)):

```sh
saida=""
if [ $s -eq 1 ]; then saida="$saida $(cat /proc/sys/kernel/ostype)"; fi
if [ $n -eq 1 ]; then saida="$saida $(hostname)"; fi
if [ $r -eq 1 ]; then saida="$saida $(cat /proc/sys/kernel/osrelease)"; fi
if [ $v -eq 1 ]; then saida="$saida $(cat /proc/sys/kernel/version)"; fi
if [ $m -eq 1 ]; then saida="$saida $maquina"; fi
if [ $p -eq 1 ]; then saida="$saida $maquina"; fi
if [ $i -eq 1 ]; then saida="$saida $maquina"; fi
if [ $o -eq 1 ]; then saida="$saida GNU/Linux"; fi

echo "${saida# }"
```

Repare que são **os mesmos comandos do seu código** (`cat /proc/...`,
`hostname`, `GNU/Linux`). A diferença é que, em vez de irem direto para a tela,
o resultado vai sendo **acumulado** na variável `saida`.

Linha por linha:

- `saida=""` cria a variável vazia.
- Cada `if` diz: "se a bandeirinha estiver em 1, coloque um espaço e a
  informação **no final** da `saida`". O `$( )` roda o comando e pega o que
  ele imprimiria.
- Os `if` estão escritos **na ordem do `uname`** (s, n, r, v, m, p, i, o). Como
  o script passa por eles sempre de cima para baixo, a ordem fica certa, não
  importa como as opções foram digitadas.
- Uma opção repetida (`-s -s`) só coloca 1 na bandeirinha duas vezes, e ela
  continua valendo 1. O `if` dela roda uma vez só.

Veja o que acontece com `-r -s`:

| Passo | Valor da `saida` |
|---|---|
| começo | `""` |
| `if` do `s` (vale 1) | `" Linux"` |
| `if` do `n` (vale 0) | `" Linux"` (não muda) |
| `if` do `r` (vale 1) | `" Linux 7.0.0-38-generic"` |
| resto (valem 0) | `" Linux 7.0.0-38-generic"` |

O `s` entrou antes do `r`, mesmo você tendo digitado `-r` primeiro.

**E o espaço sobrando no começo?** Cada campo entra com um espaço na frente,
então a `saida` começa com um espaço. O `${saida# }` na última linha tira esse
espaço:

```
saida=" Linux 7.0.0-38-generic"
echo "${saida# }"     →     Linux 7.0.0-38-generic
```

O `#` dentro de `${ }` quer dizer "tire do começo". Aqui, tire um espaço.

**E o seu comando do `-m`?** Ele é o mesmo, só que agora é **guardado** em vez
de impresso ([linha 82](new_uname_corrigido.sh#L82)):

**Antes:**

```sh
echo $(LC_ALL=C lscpu | awk '/^Architecture:/ {print $2}')
```

**Depois:**

```sh
maquina=$(LC_ALL=C lscpu | awk '/^Architecture:/ {print $2}')
```

Ele vira a variável `maquina` porque é usado em três `if` (o do `-m`, o do `-p`
e o do `-i`). O `echo $( )` que você tinha não estava errado, mas era
desnecessário: o `awk` sozinho já imprime.

---

## Mudança 3 — "Sem opção, mostra Linux" mudou de lugar

**Antes (no começo do seu código):**

```sh
if [ "$#" -eq 0 ]; then
    cat /proc/sys/kernel/ostype
    exit 0
fi
```

**Depois (logo após ler as opções,
[linhas 78–80](new_uname_corrigido.sh#L78-L80)):**

```sh
if [ "$s$n$r$v$m$p$i$o" = "00000000" ]; then
    s=1
fi
```

Duas coisas mudaram:

1. **Não imprime mais aqui**, só levanta a bandeirinha `s`. Quem imprime é o
   final do script (Mudança 2), como em todos os outros casos.
2. **A pergunta mudou.** A sua perguntava "veio algum argumento?" (`$#`). A
   nova pergunta "**alguma opção foi pedida?**". A diferença aparece em
   `uname --`: veio um argumento (`$#` vale 1), mas nenhuma opção, e o `uname`
   de verdade mostra `Linux`.

**O truque do `"00000000"`:** o `"$s$n$r$v$m$p$i$o"` **cola** os oito valores
num texto só. Se nenhuma bandeirinha foi levantada, o texto é `00000000`. Se,
por exemplo, só o `m` foi pedido, o texto é `00001000`. É um jeito curto de
perguntar "estão todas em 0?" sem escrever oito comparações.

---

## Mudança 4 — `getopts` no lugar do `while`/`shift`

Esta é a mudança que mais muda a cara do código, mas a ideia é simples.

### O problema

| Comando | `uname` de verdade | Seu script |
|---|---|---|
| `-sr` | `Linux 7.0.0-38-generic` | `new_uname: opção não existe -- '-sr'` |
| `--` | `Linux` | `new_uname: opção não existe -- '--'` |

O `uname` aceita **opções juntas**: `-sr` é o mesmo que `-s -r`, e `-snrv` é o
mesmo que `-s -n -r -v`. E o `--` significa "acabaram as opções".

### Por que acontece

O seu `case` compara o **argumento inteiro**. O texto `-sr` não é igual a
`'-s'` nem a `'-r'`, então cai no `*)`. Para aceitar `-sr` com o seu `while`,
seria preciso quebrar o texto letra por letra na mão.

### A correção

O shell já tem um comando feito exatamente para isso: o **`getopts`**. Pense
nele como **o seu `while` com `shift`, só que já sabendo separar as letras**.

**Antes (seu código):**

```sh
while [ "$#" -gt 0 ]; do
    case "$1" in
        '-s')
            ...
            ;;
        '-r')
            ...
            ;;
    esac
    shift
done
```

**Depois (corrigido, [linhas 49–66](new_uname_corrigido.sh#L49-L66)):**

```sh
while getopts ":asnrvmpio" opcao; do
    case "$opcao" in
        s) s=1 ;;
        r) r=1 ;;
        ...
    esac
done
```

Comparando pedaço por pedaço:

| Seu código | Corrigido | O que mudou |
|---|---|---|
| `while [ "$#" -gt 0 ]` | `while getopts ":asnrvmpio" opcao` | o laço continua enquanto o `getopts` achar opções |
| `case "$1"` | `case "$opcao"` | o `getopts` coloca **a letra** da vez na variável `opcao` |
| `'-s')` | `s)` | o `getopts` entrega só a letra, **sem o traço** |
| `shift` | (não tem) | o `getopts` avança sozinho |

O `"asnrvmpio"` é a **lista de letras aceitas**. (O `:` no começo é da Mudança 6.)

Veja o que o `getopts` faz com `-sr -m`, uma volta do `while` por vez:

| Volta | Valor de `opcao` | O que o `case` faz |
|---|---|---|
| 1 | `s` | `s=1` |
| 2 | `r` | `r=1` |
| 3 | `m` | `m=1` |
| 4 | acabaram as opções, então o `while` termina | |

O `-sr` foi quebrado em `s` e `r` sozinho. E, com `--`, o `getopts` entende que
ali acabaram as opções e para. Nenhuma bandeirinha foi levantada, então a
Mudança 3 entra em ação e mostra `Linux`, igual ao `uname`.

---

## Mudança 5 — Separar opção de operando

### O problema

| Comando | `uname` de verdade | Seu script |
|---|---|---|
| `-x` | `uname: invalid option -- 'x'` | `new_uname: opção não existe -- '-x'` |
| `foo` | `uname: extra operand ‘foo’` | `new_uname: opção não existe -- 'foo'` |

O `uname` de verdade tem **dois erros diferentes**:

- **opção inválida**: algo que começa com `-` mas não é uma opção conhecida
  (`-x`);
- **operando extra**: algo que **não** começa com `-` (`foo`). O `uname` não
  aceita nenhum argumento desse tipo.

O seu `*)` tratava os dois como o mesmo erro.

### A correção

O **opção inválida** fica dentro do laço do `getopts` (Mudança 6).

Para o **operando extra**, o corrigido usa um comportamento do `getopts`: ele
**para no primeiro argumento que não é opção**. Então, depois do laço, se
**sobrou** algum argumento, é um operando
([linhas 70–75](new_uname_corrigido.sh#L70-L75)):

```sh
shift $((OPTIND - 1))
if [ "$#" -gt 0 ]; then
    echo "uname: extra operand ‘$1’" >&2
    echo "Try 'uname --help' for more information." >&2
    exit 1
fi
```

- **`OPTIND`**: o `getopts` guarda nessa variável a **posição do próximo
  argumento** que ele ainda não leu. Então `OPTIND - 1` é **quantos argumentos
  eram opções**.
- **`$(( ))`** faz a conta.
- **`shift`** você já conhece. Aqui ele descarta de uma vez todos os argumentos
  que eram opções.
- Se ainda sobrou algo (`$#` maior que 0), é um operando, e o script mostra o
  erro.

Veja com `-s foo`:

1. o `getopts` lê o `-s` e para no `foo`. O `OPTIND` fica em 2;
2. `shift $((2 - 1))` = `shift 1`, que descarta o `-s`;
3. sobra o `foo`: `$#` vale 1 e `$1` vale `foo`;
4. como `$#` é maior que 0, o script mostra `uname: extra operand ‘foo’`.

---

## Mudança 6 — Mensagens de erro iguais às do `uname`

O enunciado pede "mensagens de erro e saídas correspondentes" ao comando
original.

**Antes (seu código):**

```
new_uname: opção não existe -- '-x'
```

**`uname` de verdade (e o corrigido):**

```
uname: invalid option -- 'x'
Try 'uname --help' for more information.
```

As diferenças:

1. o nome é `uname`, e não `new_uname`;
2. o texto está em inglês, como o `uname` desta máquina;
3. aparece só a letra (`'x'`), **sem o traço**;
4. tem uma **segunda linha**, o `Try 'uname --help'...`.

O código ([linhas 60–64](new_uname_corrigido.sh#L60-L64)):

```sh
*)
    echo "uname: invalid option -- '$OPTARG'" >&2
    echo "Try 'uname --help' for more information." >&2
    exit 1
    ;;
```

**De onde vem a letra no `$OPTARG`?** Do `:` no começo de `":asnrvmpio"`. Esse
`:` liga o "modo silencioso" do `getopts`. Quando aparece uma letra que não está
na lista, ele:

- **não imprime** a mensagem de erro dele (que seria `Illegal option -x`,
  diferente da do `uname`);
- coloca `?` na variável `opcao`, que cai no `*)`;
- coloca a letra inválida na variável **`OPTARG`**.

Assim, quem imprime a mensagem é o script, com o texto idêntico ao do `uname`.
O `>&2` e o `exit 1` são os mesmos que você já usava.

Na mensagem de operando extra (Mudança 5), as aspas são **curvas** (`‘foo’`), e
não retas. Não é erro de digitação: o `uname` de verdade usa esse tipo de
aspas nessa mensagem.

---

## O que foi só acrescentado (não é correção)

Estas partes **não consertam nada** do seu código. São opções novas, para o
trabalho passar de 5 parâmetros e ganhar o bônus:

- **`-a`**: mostra tudo. No `case`, ele levanta todas as bandeirinhas de uma vez:
  ```sh
  a) s=1; n=1; r=1; v=1; m=1; p=1; i=1; o=1 ;;
  ```
- **`-p` e `-i`**: tipo do processador e plataforma de hardware. No Ubuntu, os
  dois mostram a mesma arquitetura do `-m`, então usam a variável `maquina`. Por
  isso o `uname -a` mostra `x86_64` três vezes.
- **`--help` e `--version`**
  ([linhas 8–45](new_uname_corrigido.sh#L8-L45)): o `getopts` só entende opções
  de **uma letra**, então essas duas são checadas **antes** do laço, olhando o
  primeiro argumento (`$1`). O texto é copiado do original e impresso com
  `cat <<'FIM'`. Esse comando imprime tudo o que vem nas linhas seguintes até a
  linha que diz `FIM`; é um jeito prático de imprimir um texto grande.

---

## Resumo: antes e depois

| Comando | Seu código | Corrigido |
|---|---|---|
| `-s`, `-r`, `-v`, `-m`, `-n`, `-o` sozinhas | ✅ | ✅ |
| sem argumento | ✅ | ✅ |
| `-s -r` (uma linha) | ❌ duas linhas | ✅ |
| `-r -s` (ordem fixa) | ❌ ordem trocada | ✅ |
| `-s -s` (sem repetir) | ❌ repete | ✅ |
| `-s -x` (só o erro) | ❌ imprime `Linux` antes | ✅ |
| `-sr` (opções juntas) | ❌ erro | ✅ |
| `--` | ❌ erro | ✅ |
| `-x` (mensagem) | ❌ mensagem diferente | ✅ |
| `foo` (mensagem) | ❌ mensagem diferente | ✅ |
| `-a`, `-p`, `-i`, `--help`, `--version` | não existia | ✅ acrescentado |

---

## Teste você mesmo

Dentro da pasta `Lista SemiFinal`, rode cada trio e compare:

```
uname -r -s
sh new_uname.sh -r -s
sh new_uname_corrigido.sh -r -s

uname -sr
sh new_uname.sh -sr
sh new_uname_corrigido.sh -sr

uname -s -x
sh new_uname.sh -s -x
sh new_uname_corrigido.sh -s -x

uname foo
sh new_uname.sh foo
sh new_uname_corrigido.sh foo
```

Em todos eles, a primeira e a terceira saída são iguais.
