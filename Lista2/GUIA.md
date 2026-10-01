# Guia da Lista 2 — do zero

Para quem nunca viu a matéria. Lê na ordem; dá pra fazer em uns 20 minutos.

---

## 1. O trabalho em uma frase

Fazer um programa que **fica rodando escondido** e, de tantos em tantos segundos,
**anota num arquivo** quais processos do sistema estão "zumbis".

---

## 2. Os conceitos (o mínimo pra entender)

**Processo** é um programa em execução. Abriu o navegador? Virou um processo.
Cada um tem um número de identidade, o **PID**.

**Todo processo tem um pai.** Quem criou ele. O número do pai é o **PPID**.
É assim que o sistema vira uma árvore de famílias.

**`fork()` é a única forma de criar processo no Unix.** E ele é esquisito:
clona o programa em dois. A partir dali existem **duas cópias rodando o mesmo
código**, lado a lado. Como diferenciar quem é quem? Pelo que o `fork()` devolve:

| Quem | O `fork()` devolve |
|---|---|
| no **pai** | o PID do filho (um número grande, nunca zero) |
| no **filho** | **0** |

Por isso `if (fork())` significa na prática *"só o pai entra aqui"* — porque no
filho o valor é 0, que em C é falso.

**O que é um processo ZUMBI** (o assunto central):

Quando um filho morre, ele não some na hora. O sistema guarda **uma fichinha**
dele dizendo "morri, e morri assim". Essa ficha fica esperando o pai perguntar
"e aí, como foi?" (isso se chama dar `wait`). Enquanto o pai **não pergunta**, a
ficha fica lá parada. **Essa ficha é o zumbi**: o processo já morreu, mas ainda
aparece na lista do sistema.

> Analogia: o aluno entregou a prova e foi embora. A prova fica na mesa do
> professor até ele corrigir. O aluno não está mais lá, mas tem papel dele
> na mesa. O papel é o zumbi.

Zumbi **não gasta memória nem processador** — é só a fichinha. Mas se acumular
muito, enche a tabela de processos do sistema. No `ps` ele aparece com a letra
**`Z`** ou escrito `<defunct>`.

**Daemon** (lê-se "dímon") é um programa que roda **em background**, sem janela,
sem terminal, sem você ver. Fica lá fazendo o trabalho dele sozinho.

**Sinal** é um recadinho que o sistema manda pra um processo. Os que importam aqui:

| Sinal | O que significa | Dá pra ignorar? |
|---|---|---|
| `SIGINT` | o Ctrl+C | sim |
| `SIGTERM` | "por favor, termina" (educado) | sim |
| `SIGKILL` | "morre agora" (`kill -9`) | **não, nunca** |
| `SIGHUP` | o terminal foi fechado | sim |
| `SIGCHLD` | "um filho seu morreu" | sim |

Pra cada sinal o programa escolhe: **ignorar** (`SIG_IGN`) ou **tratar**
(apontar uma função sua, chamada *handler*, que roda quando o sinal chega).

**Pipe** é um cano que liga a saída de um programa à entrada de outro.
A função **`popen()`** faz isso pronto: roda um comando e te devolve a saída
dele **como se fosse um arquivo** que você lê linha por linha.

**`ps`** é o comando que lista os processos do sistema.

---

## 3. O que o PDF pede (a lista de conferência)

1. Um **daemon** (roda em background)
2. Acorda de **n em n segundos**, sendo o `n` passado na linha de comando
3. Escreve **num arquivo de log próprio** quem são os zumbis
4. O log tem que ter **PID, PPID e nome do programa**, com linhas de `=====`
   separando cada leitura
5. Descobrir os zumbis **rodando o `ps` como filho e lendo por um pipe**
   (o PDF aceita outra forma, varrendo o `/proc`, mas a gente usou essa)
6. Ao receber **SIGTERM**, escrever uma mensagem de despedida no log e terminar
7. Ser **invulnerável a todos os outros sinais** (menos o SIGKILL, que ninguém
   segura)

---

## 4. São dois programas

- **`gerazumbi.c`** — vem pronto no PDF. Só serve pra **fabricar zumbis**,
  senão não tem o que o daemon encontrar.
- **`lista2_ajustado.c`** — o trabalho de verdade, o daemon.

---

## 5. `gerazumbi.c` — o que cada trecho faz

*"Ele cria filhos que morrem na hora e nunca pergunta como eles morreram.
Por isso as fichinhas ficam lá, e eu tenho zumbis."*

> Esta seção é resumida de propósito, porque este programa não é o trabalho.
> Se travar em alguma coisa aqui (o que é "argumento", o `atol`, o `? :`, o
> `fork`), está tudo mastigado na seção 6, que explica o mesmo com calma.

- `(n = (argc == 1) ? 1 : atol(argv[1])) <= 0` — se não veio argumento usa 1,
  senão converte o texto em número (`atol`). Se der zero ou negativo, reclama e sai
- `if (fork()) exit(0);` — **vai pro background**. Clona; o pai sai na hora. O
  filho continua sozinho, sem ninguém esperando por ele, e o terminal te devolve
  o prompt na hora
- `while (n-- > 0) { if (fork() == 0) exit(0); }` — cria `n` filhos; cada filho
  (que é quem recebe 0) **morre imediatamente** → vira zumbi
- `for (EVER) pause();` — o `EVER` é só um apelido pro `;;`, definido lá em cima
  com `#define`. `pause()` dorme pra sempre. O pai fica vivo **sem nunca dar
  `wait`**, e é exatamente isso que mantém os zumbis existindo

---

## 6. `lista2_ajustado.c` — linha por linha

Nada é pulado aqui. Vai na ordem do arquivo.

> Se da linha 36 em diante ficar pesado, a **seção 9** explica a mesma parte
> final de forma bem mais curta e direta. As duas dizem a mesma coisa.

### 6.1 As quatro primeiras linhas

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
```

`#include` é "me empresta essa caixa de ferramentas". Sozinho, o C não sabe
abrir arquivo nem criar processo. Esqueceu uma caixa, nem compila.

| Caixa | O que usamos dela |
|---|---|
| `stdio.h` | ler e escrever: `fopen`, `fprintf`, `fgets`, `popen` |
| `stdlib.h` | `exit`, `atol` |
| `signal.h` | `signal`, `SIG_IGN`, `SIGTERM` |
| `unistd.h` | coisas do Unix: `fork`, `sleep` |

### 6.2 A variável do arquivo

```c
FILE *arquivo_log;
```

Ao abrir um arquivo, o C te devolve **um crachá** dele — não o arquivo inteiro,
só uma etiqueta que identifica "o arquivo aberto ali". Esse crachá tem tipo
`FILE *`; a estrelinha diz que é um endereço, um bilhete com o número da sala
em vez da sala inteira.

O que importa: essa linha está **fora de qualquer função**, no alto do arquivo.
Isso faz dela uma **variável global**, que todo mundo enxerga. Precisa ser
assim porque **duas** partes usam o crachá: o `main` e a função que trata o
sinal. Dentro do `main`, a outra função não conseguiria ver.

### 6.3 A função que atende o sinal

```c
void trata_sinal(int sig)
{
    fprintf(arquivo_log, "Recebi um sinal para fechar\n");
    fclose(arquivo_log);
    exit(0);
}
```

Esta função tem uma coisa esquisita: **você nunca a chama**. Procura no resto
do programa, não existe nenhum `trata_sinal()` escrito. Quem chama é o
**sistema operacional**, sozinho, quando o SIGTERM chegar. Mais à frente a
gente registra ela — é deixar um telefone de plantão anotado.

- `int sig` — o sistema passa **o número do sinal** que chegou. A gente não usa,
  mas a função tem que aceitar, porque é o formato que o sistema exige
- `fprintf(arquivo_log, "...")` — é o `printf`, só que escrevendo **dentro do
  arquivo** do crachá em vez de na tela. O `\n` é a quebra de linha
- `fclose` — fecha o arquivo direito, garantindo que o que estava pendente seja
  gravado mesmo
- `exit(0)` — encerra o programa **inteiro**, na hora. O `0` quer dizer "terminei
  bem" (no Unix, zero é sucesso, qualquer outro número é problema)

É isto que cumpre o pedido do enunciado: SIGTERM interceptado, com mensagem de
despedida no log.

### 6.4 Recebendo o número de segundos

```c
int main(int argc, char **argv)
{
    int n, i;
    char linha[256];
```

**O que é "argumento":** é o que você digita **depois** do nome do programa.
Em `./lista2_ajustado 3`, esse `3` é um argumento. O sistema corta a linha nos
espaços e entrega os pedaços em duas variáveis:

| | O que é | Em `./lista2_ajustado 3` |
|---|---|---|
| `argc` | **quantas** palavras foram digitadas | `2` |
| `argv[0]` | a 1ª palavra (o nome do programa) | `"./lista2_ajustado"` |
| `argv[1]` | a 2ª palavra | `"3"` |

Digitando só `./lista2_ajustado`, sem nada depois, `argc` vale **1** e
`argv[1]` não existe.

As variáveis: `n` guarda o intervalo em segundos, `i` é contador do laço dos
sinais, e `linha` é uma **caixinha com espaço pra 256 letras** onde cada linha
lida do `ps` fica um instante antes de ir pro log.

```c
    if ((n = (argc == 1) ? 1 : atol(argv[1])) <= 0)
    {
        fprintf(stderr, "Use: %s [<n>]\n", argv[0]);
        return 1;
    }
```

Parece monstruosa, mas são três coisas empilhadas. De dentro pra fora:

**1) `(argc == 1) ? 1 : atol(argv[1])`** — um `if` comprimido (operador
ternário). Lê-se: *"`argc` é igual a 1? Se sim, o resultado é `1`; se não, é
`atol(argv[1])`"*. Do jeito longo seria:

```c
if (argc == 1)
    resultado = 1;
else
    resultado = atol(argv[1]);
```

**Por que 1 quando não vem nada?** Porque o programa precisa de **algum**
intervalo. Se esquecerem de digitar o número, em vez de dar erro ele assume 1
segundo e roda. É o mesmo que o programa do enunciado faz.

**O que é `atol`:** `argv[1]` é **texto**, não número. Você digitou o caractere
`3`, e pro computador isso é um desenho, uma letra — ele não sabe que vale três.
`atol` traduz texto pra número de verdade, que é o que o `sleep` precisa.

**2) `n = (...)`** — guarda o resultado em `n`. Em C uma atribuição também
**vale** o valor atribuído, e é esse truque que permite o passo 3.

**3) `(...) <= 0`** — compara com zero o que acabou de ser guardado. Entra no
`if` e reclama quando:

- você digitou `./lista2_ajustado abc` → `atol` não traduz e devolve 0
- você digitou `./lista2_ajustado -5` → negativo

Nos dois casos não dá pra dormir essa quantidade de segundos.

- `fprintf(stderr, ...)` — `stderr` é o **canal de erros**, separado da saída
  normal. É a tubulação certa pra mensagem de problema
- `%s` é um buraco preenchido pelo que vem depois, aqui o `argv[0]`. Assim a
  mensagem sai com o nome que **você** usou pra chamar o programa
- `return 1` — encerra o `main`; o `1` avisa que terminou **mal**

### 6.5 Abrindo o arquivo de log

```c
    arquivo_log = fopen("zumbie.txt", "a+");
    if (arquivo_log == NULL)
    {
        perror("zumbie.txt");
        return 1;
    }
```

`fopen` abre o arquivo e devolve o crachá; se não existir, cria. O `"a+"` é o
**modo**, e a escolha importa:

| Modo | O que faz |
|---|---|
| `"w"` | **apaga tudo** e começa do zero |
| `"a"` | *append*: escreve **no fim**, preservando o que tinha |
| `"a+"` | igual ao `"a"`, e ainda deixa ler |

Com `"w"` o log seria apagado toda vez que o daemon subisse.

- `NULL` é o jeito do C dizer "nada". Quando o `fopen` não consegue abrir (pasta
  só de leitura, disco cheio), devolve `NULL` no lugar do crachá
- **sem essa checagem o programa quebraria feio** lá na frente, ao tentar
  escrever num crachá que não existe
- `perror` imprime a mensagem junto com o motivo que o sistema deu, tipo
  `zumbie.txt: Permission denied`

### 6.6 Indo pro background

```c
    if (fork())
        exit(0);
```

Duas linhas e o programa vira daemon:

1. `fork()` **clona** o programa. Agora são dois rodando o mesmo código, parados
   nesta mesma linha
2. no **pai**, o `fork()` devolveu o PID do filho — número grande, diferente de
   zero. Em C, diferente de zero é verdadeiro, então o pai **entra** no `if` e
   executa `exit(0)`: morre
3. no **filho**, o `fork()` devolveu `0`, que é falso. Ele **não entra** e segue
   o programa

O terminal estava esperando o **pai**. O pai morreu, então o prompt volta na
hora. O filho continua rodando sem terminal nenhum atrás dele, adotado pelo
sistema. Isso é estar em background.

### 6.7 Blindando contra os sinais

```c
    for (i = 1; i < NSIG; i++)
        if (i != SIGCHLD)
            (void)signal(i, SIG_IGN);
    (void)signal(SIGTERM, trata_sinal);
```

**Primeiro o modelo mental.** O sistema mantém, **para cada programa rodando**,
um caderninho de recados. Uma linha por sinal, dizendo o que fazer quando aquele
sinal chegar. Cada sinal tem um número, e os nomes tipo `SIGTERM` são só apelidos
pra esses números. O caderninho do seu programa começa assim, de fábrica:

| nº | apelido | o que o sistema faz, de fábrica |
|---|---|---|
| 1 | `SIGHUP` | **mata o programa** |
| 2 | `SIGINT` | **mata o programa** |
| 9 | `SIGKILL` | **mata o programa** |
| 10 | `SIGUSR1` | **mata o programa** |
| 15 | `SIGTERM` | **mata o programa** |
| 17 | `SIGCHLD` | não faz nada |
| ... | ... | quase tudo: **mata o programa** |

É por isso que a primeira versão do código morria quando a gente mandava
`kill -USR1`: ninguém tinha reescrito a linha 10 do caderninho, então valia o de
fábrica, que é matar.

**O que a função `signal` faz:** reescreve **uma linha** do caderninho. Ela
recebe dois ingredientes — qual linha (o número do sinal) e o que passar a
fazer. O "o que fazer" pode ser:

- `SIG_IGN` → *"não faz nada, finge que não chegou"*
- o nome de uma função sua → *"chama essa função"*

**Agora o laço.** `for (i = 1; i < NSIG; i++)` significa: *"começa com `i`
valendo 1; enquanto `i` for menor que `NSIG`, faz o que está embaixo; a cada
volta soma 1 no `i`"*. `NSIG` é um nome pronto que vale quantos sinais existem
(nesta máquina, 65). Começa em 1 porque não existe sinal de número zero.

Então, na prática, o laço faz isto:

```
volta 1  → i = 1  → reescreve a linha  1 (SIGHUP):  ignorar
volta 2  → i = 2  → reescreve a linha  2 (SIGINT):  ignorar
volta 3  → i = 3  → reescreve a linha  3 (SIGQUIT): ignorar
...
volta 17 → i = 17 → PULA, por causa do if
...
volta 64 → i = 64 → reescreve a última linha: ignorar
```

Em vez de escrever 60 linhas de `signal` na mão, uma a uma, o laço cobre todas.
E cobrir **todas** é literalmente o que o enunciado pede.

Os detalhes que sobram:

- `if (i != SIGCHLD)` — `!=` quer dizer "diferente de". Quando o contador chega
  em 17, essa volta é pulada e a linha do `SIGCHLD` fica como estava. O SIGCHLD
  é o aviso "um filho seu morreu", e o daemon cria um filho a cada rodada (o
  `ps`), então é mais correto não mexer nesse aviso. (Testando, funciona dos
  dois jeitos — mas é mais correto assim)
- **`SIGKILL` (9) e `SIGSTOP` (19) o sistema não deixa reescrever.** O laço vai
  tentar, e o sistema simplesmente ignora o pedido nesses dois. Não dá erro, não
  trava nada — a linha deles continua dizendo "mata". E está certo: o enunciado
  já avisa que contra o SIGKILL ninguém resiste
- `(void)` na frente é só um aviso pra quem lê o código: *"eu sei que a função
  `signal` devolve um valor e estou jogando fora de propósito"*. Não muda nada
  no funcionamento

**A última linha, e por que a ordem importa.** O laço acabou de mandar ignorar
**tudo** — inclusive o SIGTERM, que é a linha 15. Aí vem:

```c
    (void)signal(SIGTERM, trata_sinal);
```

Ela reescreve a linha 15 **de novo**, agora trocando "ignorar" pelo nome da
nossa função. Repare que não tem parênteses depois de `trata_sinal`: você não
está *chamando* a função, está entregando o **nome** dela pro sistema guardar e
chamar depois, quando o sinal chegar.

Como essa linha vem **depois** do laço, ela escreve por cima. Se estivesse antes,
o laço apagaria o que ela fez e o daemon ficaria impossível de encerrar.

O caderninho termina assim:

| nº | apelido | o que o sistema faz agora |
|---|---|---|
| 1 | `SIGHUP` | ignora |
| 2 | `SIGINT` | ignora |
| 9 | `SIGKILL` | mata — o sistema recusou a mudança |
| 10 | `SIGUSR1` | ignora |
| 15 | `SIGTERM` | **chama `trata_sinal`** |
| 17 | `SIGCHLD` | continua de fábrica (foi pulado) |
| ... | ... | ignora |

Que é exatamente o pedido do enunciado: invulnerável a tudo, menos ao SIGKILL,
e encerrando pelo SIGTERM com mensagem no log.

### 6.8 O cabeçalho do log

```c
    fprintf(arquivo_log, "PID PPID Nome do Programa\n");
    fflush(arquivo_log);
```

São duas coisas diferentes. Uma de cada vez.

**A primeira linha** escreve o título das colunas, no formato que o PDF mostra.
Ela está **fora** do `while` que vem logo abaixo — e isso é de propósito. Tudo
que está dentro do `while` se repete pra sempre; esta linha está fora, então
roda **uma vez só**, quando o daemon sobe. Se estivesse dentro, o título
apareceria no log a cada rodada. O `\n` no fim é a quebra de linha; sem ele, a
próxima coisa escrita grudaria no título.

**A segunda linha precisa de uma explicação maior**, porque é bem contra a
intuição.

Quando você manda escrever num arquivo, **o texto não vai pro disco na hora**.
Ele cai num **balde na memória**. O programa vai jogando texto no balde, e só
quando o balde **enche** é que o sistema despeja tudo de uma vez no arquivo de
verdade. Isso existe por velocidade: escrever no disco é lento, então vale a
pena juntar um monte e gravar tudo junto.

O problema é o tamanho do balde. Medindo aqui nesta máquina:

```
o balde comporta 4096 caracteres
uma linha do log ("29485 29484 gerazumbi") tem 22 caracteres
4096 ÷ 22 ≈ 186 linhas
```

Ou seja: **sem o `fflush`, você precisaria esperar o programa escrever umas 186
linhas antes de o arquivo deixar de estar vazio.** Com 3 zumbis e o daemon
acordando de 2 em 2 segundos, cada rodada escreve pouco mais de 100 caracteres
— daria mais de um minuto olhando pra um `zumbie.txt` vazio, achando que o
programa está quebrado, quando na verdade ele está funcionando e o texto está
preso no balde.

Normalmente quem resolve isso é o `fclose`, que esvazia o balde ao fechar o
arquivo. Só que **este programa nunca fecha** — ele roda pra sempre. Então sem
o `fflush` o texto ficaria lá preso.

`fflush(arquivo_log)` quer dizer: **"esvazia o balde agora, não espera encher"**.
É o que permite dar `cat zumbie.txt` com o daemon ainda rodando e já ver o
conteúdo — que é exatamente o que você vai fazer na hora de apresentar.

### 6.9 O laço eterno

```c
    while (1)
    {
        sleep(n);
        FILE *comando = popen("ps -eo pid,ppid,comm,stat --no-headers | awk '$4 ~ /^Z/ {print $1, $2, $3}'", "r");
```

- `while (1)` — laço que roda **enquanto a condição for verdadeira**, e a
  condição é literalmente `1`, sempre verdadeiro. Ou seja: pra sempre. Quem
  interrompe é o SIGTERM
- `sleep(n)` — dorme `n` segundos sem gastar processador. É esta linha que faz o
  "de n em n segundos" do enunciado
- `popen` — o irmão do `fopen`. O `fopen` abre um **arquivo**; o `popen` roda um
  **comando** como processo filho e devolve a saída dele embrulhada como se fosse
  um arquivo, com crachá e tudo. Você lê a resposta do comando igual leria um
  texto. O `"r"` é de *read*

É aqui que a exigência do enunciado é cumprida: achar os zumbis rodando o `ps`
como filho e pegando a saída por um pipe.

O comando de dentro, em pedaços:

| Pedaço | O que faz |
|---|---|
| `ps` | lista processos |
| `-e` | **todos**, não só os seus |
| `-o pid,ppid,comm,stat` | escolhe as colunas: PID, PID do pai, nome e estado |
| `--no-headers` | tira a linha de título; sem isso ela iria pro log como se fosse um processo |
| `\|` | o **pipe**: joga a saída do `ps` como entrada do `awk` |
| `awk '$4 ~ /^Z/` | o `awk` lê linha a linha e numera as colunas `$1`, `$2`... Isto quer dizer *"fica só com as linhas cuja 4ª coluna começa com Z"*. A 4ª é o estado; Z é de zumbi |
| `{print $1, $2, $3}'` | dessas, imprime as três primeiras colunas: PID, PPID e nome — a ordem do enunciado |

```c
        if (comando != NULL)
        {
            fprintf(arquivo_log, "==========================================\n");
            while (fgets(linha, sizeof(linha), comando) != NULL)
            {
                fprintf(arquivo_log, "%s", linha);
            }
            fflush(arquivo_log);
            pclose(comando);
        }
    }
}
```

- `if (comando != NULL)` — mesma ideia do `fopen`: se o `popen` falhou devolve
  `NULL` e não há o que ler. Só entra se deu certo
- a linha de `=====` é o separador que o enunciado mostra, uma por rodada
- `fgets(linha, sizeof(linha), comando)` — lê **uma linha** da saída e guarda na
  caixinha. Os três ingredientes: onde guardar, o tamanho máximo, de onde ler.
  O `sizeof(linha)` vale 256 e serve de trava, pra nunca escrever além da
  caixinha e estourar a memória
- quando a saída acaba, o `fgets` devolve `NULL`. Por isso o `while` com
  `!= NULL` quer dizer *"vai lendo até acabar"*
- `fprintf(arquivo_log, "%s", linha)` — copia a linha pro log. Repare que **não
  tem `\n`**: a quebra já veio junto do `fgets`, que não tira. E o `"%s"` está
  ali por segurança — escrever `fprintf(arquivo_log, linha)` direto seria
  perigoso, porque o C trataria o conteúdo como formato e qualquer `%` que
  aparecesse bagunçaria tudo
- `fflush` — grava agora, igual ao de antes. É por isso que dá pra fazer
  `cat zumbie.txt` com o daemon rodando e já ver o conteúdo
- `pclose(comando)` — fecha o cano e libera o processo do `ps` daquela rodada.
  Está **dentro** do `if` de propósito: só faz sentido fechar o que foi aberto

Aí a chave fecha, o `while (1)` volta pro começo, dorme mais `n` segundos e faz
tudo de novo. Pra sempre, até chegar um SIGTERM.

---

## 7. Roteiro da demonstração

```bash
gcc -o gerazumbi gerazumbi.c
gcc -o lista2_ajustado lista2_ajustado.c

./gerazumbi 3                                    # fabrica 3 zumbis
ps -eo pid,ppid,comm,stat | awk '$4 ~ /^Z/'      # mostra que existem

./lista2_ajustado 2                              # sobe o daemon (2 em 2 segundos)
ps -eo pid,comm | grep lista2                    # mostra ele rodando sozinho
sleep 6
cat zumbie.txt                                   # o log preenchido

kill -INT  <PID>                                 # tenta matar: nao morre
kill -USR1 <PID>                                 # tenta de novo: nao morre
kill -TERM <PID>                                 # so este encerra
cat zumbie.txt                                   # a mensagem de despedida no fim
```

Pega o `<PID>` do daemon com `pgrep lista2_ajustado`. Se ele devolver **mais de
um número**, é porque sobrou daemon de um teste anterior rodando — encerra os
antigos com `pkill lista2_ajustado` e sobe um só.

---

## 8. Se perguntarem

**"Por que o `fork` sem `wait` gera zumbi?"**
Porque a fichinha do filho morto só é descartada quando o pai dá `wait`.
Sem `wait`, ela fica.

**"Por que `if (fork()) exit(0)` joga pro background?"**
O pai sai na hora e devolve o terminal. O filho continua rodando, agora adotado
pelo sistema, sem terminal nenhum atrás dele.

**"Por que ignorar todos os sinais num laço em vez de listar um por um?"**
Porque o enunciado pede invulnerável a **todos** menos o SIGKILL. Listando na
mão sempre escapa algum.

**"E se eu mandar `kill -9`?"**
Morre. E é o esperado — o SIGKILL não pode ser ignorado por ninguém, está no
enunciado.

**"Por que o daemon não vira zumbi também?"**
Ele não tem pai esperando por ele; foi adotado pelo sistema, que recolhe a ficha
automaticamente quando ele morre.

**"Por que `--no-headers`?"**
Sem isso a primeira linha do `ps` é o título das colunas e iria parar no log
como se fosse um processo.

---

## 9. Versão direta — da linha 36 até o fim

A seção 6 explica tudo com calma. Esta aqui explica **a mesma coisa** de forma
mais curta e direta, só a parte final do código, que é a que mais cai em
pergunta. Se a seção 6 pesou, lê esta.

Da linha 36 pro fim, o programa faz **três coisas**:

1. se protege dos sinais (linhas 36 a 39)
2. escreve o título no arquivo (linhas 41 e 42)
3. entra num laço que nunca termina (linhas 44 a 58)

### 9.1 Linhas 36 a 39 — se proteger

**Sinal** é um tipo de recado que mandam pro teu programa. Cada recado tem um
número: o 2 é o Ctrl+C, o 9 é o `kill -9`, o 15 é o `kill` normal. De fábrica,
quase todo recado **mata o programa**. O daemon não pode morrer.

```c
36    for (i = 1; i < NSIG; i++)
37        if (i != SIGCHLD)
38            (void)signal(i, SIG_IGN);
39    (void)signal(SIGTERM, trata_sinal);
```

O `i` é um **contador**: vale 1, depois 2, depois 3, até 64 (o último recado que
existe). A cada número, a linha 38 dá a ordem: *"recado número `i`: ignorar"*.

A linha 39 trata o recado 15 (o `SIGTERM`): em vez de ignorar, **chama a função
`trata_sinal`**. Ela vem **depois** de propósito — a linha 38 já tinha mandado
ignorar o 15, e quem fala por último manda. Se estivesse antes, o laço apagaria.

Resultado: tudo ignorado, menos o 15. É o que o enunciado pede.

### 9.2 Por que ele deixa o 17 de fora?

Cuidado com as palavras, que é onde todo mundo se perde:

| | O que significa |
|---|---|
| a linha 38 | dá uma **ordem**: "quando chegar, não faça nada" |
| a linha 37 | faz o programa **não dar ordem nenhuma** sobre o 17 |

Então o 17 não é "ignorado" — ele fica **como veio de fábrica**, sem o programa
mexer.

E por que justo ele? Porque o recado 17 é o único em que a palavra "ignorar" tem
um **segundo significado escondido**. Nos outros, ignorar é só "não faça nada".
No 17, ignorar quer dizer pro sistema: *"nem me avise quando meus filhos
morrerem, pode limpar sozinho"*.

Isso atrapalharia, porque a cada ronda o daemon cria um filho (o `ps`, na linha
47) e depois vai recolher ele (o `pclose`, na linha 56). Se o sistema já tivesse
limpado por conta própria, o `pclose` chegaria e não acharia nada.

> **Se perguntarem:** *"deixo o SIGCHLD de fora porque pra ele o 'ignorar'
> significa também 'limpe meus filhos sozinho', e isso atrapalharia o `pclose`
> que eu uso a cada rodada."*
>
> (Testando, funciona dos dois jeitos — mas é mais correto assim.)

### 9.3 Linhas 41 e 42 — escrever o título

```c
41    fprintf(arquivo_log, "PID PPID Nome do Programa\n");
42    fflush(arquivo_log);
```

A 41 escreve o título das colunas, uma vez só. A 42 força a gravação: o C não
grava na hora, ele junta o texto na memória e só grava quando acumula bastante.
`fflush` quer dizer **"grava agora"**. Sem isso você abriria o `zumbie.txt` e
veria vazio.

### 9.4 Por que existe um laço?

Porque o enunciado pede um programa que fica **vigiando**: *"de n em n segundos
acorda e escreve"*. Sem laço, o programa olharia os zumbis uma vez e morreria.

Pensa num **vigia noturno**:

1. dorme um pouco
2. acorda e dá uma volta pra ver quem são os zumbis
3. anota no caderno
4. volta pro passo 1

A noite inteira, até mandarem ele ir embora. O laço é essa rotina.

### 9.5 São dois laços, e eles fazem coisas diferentes

O esqueleto abaixo está **resumido de propósito** (algumas linhas trocadas por
texto em português) só pra você enxergar o formato — o código de verdade está
na 9.6:

```
44    while (1)                          ← laço de FORA
46        sleep(n);
47        comando = popen(...)
50        escreve =====
51        while (fgets(...) != NULL)     ← laço de DENTRO
53            copia uma linha pro arquivo
55        grava
56        fecha
```

- **o de fora (linha 44)** é a rotina do vigia. Uma volta = uma ronda. Nunca
  termina
- **o de dentro (linha 51)** acontece **durante uma única ronda**. A resposta do
  comando pode ter várias linhas, e o C só pega uma por vez — então ele pega
  uma, copia, pega outra, copia, até a resposta acabar

Com 3 zumbis no sistema:

```
RONDA 1 (volta 1 do laço de fora)
   dorme 2 segundos
   pergunta pro ps quem sao os zumbis  → a resposta tem 3 linhas
   escreve =====
   laço de dentro roda 3 vezes:  copia linha 1
                                 copia linha 2
                                 copia linha 3
   acabou a resposta → o laço de dentro para
   grava e fecha

RONDA 2 (volta 2 do laço de fora)
   dorme 2 segundos
   ... tudo de novo, pra sempre
```

O laço de dentro para quando a resposta acaba. O laço de fora **não para com
nada** — só com o `kill -TERM`.

### 9.6 Dentro da ronda, linha por linha

**Linha 46 — dormir**

```c
46        sleep(n);
```

`n` vale 2 (foi o que você digitou). Para aqui 2 segundos, sem gastar
processador. É só esta linha que faz o "de n em n segundos".

**Linha 47 — perguntar quem são os zumbis**

```c
47        FILE *comando = popen("ps -eo pid,ppid,comm,stat --no-headers | awk '$4 ~ /^Z/ {print $1, $2, $3}'", "r");
```

A linha mais importante do programa. Três perguntas:

*Por que rodar um comando?* Porque o C não tem função pronta tipo "me dá a lista
de zumbis". Não existe. Mas o terminal tem o `ps`, que lista processos. Então o
programa faz o que você faria na mão: roda o `ps` e lê a resposta.

*O que o `popen` faz?* Roda o comando **como processo filho** e devolve a
resposta **como se fosse um arquivo**. Você lê igual leria um texto. O `"r"` é
de *read*. Isto cumpre o pedido do enunciado: o `ps` como filho, a saída por um
pipe.

*E o comando?* São dois programas ligados. Primeiro o `ps` lista **todos** os
processos — numa máquina comum isso dá umas **371 linhas**:

```
      1       0 systemd         Ss
      2       0 kthreadd        S
  54788   54787 gerazumbi       Z     ← zumbi
  54789   54787 gerazumbi       Z     ← zumbi
```

As colunas são PID, PID do pai, nome e **estado**. O estado é a letra do fim:
`S` dormindo, `R` rodando, **`Z` zumbi**.

O `|` é o **pipe**: joga essas 371 linhas dentro do `awk`. O `awk` lê linha por
linha e numera as colunas sozinho (`$1`, `$2`, `$3`, `$4`). Então:

- `$4 ~ /^Z/` → *"fica só com as linhas cuja 4ª coluna começa com Z"*
- `{print $1, $2, $3}` → *"dessas, imprime as três primeiras colunas"*

Das 371 linhas sobram 3:

```
54788 54787 gerazumbi
54789 54787 gerazumbi
54790 54787 gerazumbi
```

PID, PPID e nome, na ordem do enunciado. **Essa é a resposta pra "como você
identifica um zumbi?"**: pelo estado `Z` na saída do `ps`.

**Linha 48 — conferir se deu certo**

```c
48        if (comando != NULL)
```

Se o `popen` não conseguiu rodar, devolve `NULL` ("nada") e não há resposta pra
ler. Esta linha é um porteiro: só entra se deu certo.

**Linha 50 — o separador**

```c
50            fprintf(arquivo_log, "==========================================\n");
```

A fileira de `=`, uma por ronda. Separa uma leitura da outra, como o PDF mostra.

**Linhas 51 a 54 — copiar a resposta pro arquivo**

```c
51            while (fgets(linha, sizeof(linha), comando) != NULL)
52            {
53                fprintf(arquivo_log, "%s", linha);
54            }
```

O `fgets` tem três ingredientes, nesta ordem:

| Ingrediente | O que é |
|---|---|
| `linha` | a caixinha onde guardar (cabe 256 letras) |
| `sizeof(linha)` | o tamanho da caixinha, 256 — uma **trava** pro texto nunca passar do tamanho e estourar a memória |
| `comando` | de onde ler: a resposta do `ps` |

Em câmera lenta, o conteúdo da caixinha:

```
1ª volta:  fgets pega a 1ª linha → caixinha: "54788 54787 gerazumbi"
           a linha 53 copia pro arquivo
2ª volta:  fgets pega a 2ª linha → caixinha: "54789 54787 gerazumbi"
           a linha 53 copia pro arquivo
3ª volta:  fgets pega a 3ª linha → caixinha: "54790 54787 gerazumbi"
           a linha 53 copia pro arquivo
4ª volta:  acabou a resposta     → fgets devolve NULL
           o while vê o NULL e PARA
```

A caixinha é **reaproveitada**: a cada volta o novo conteúdo escreve por cima do
antigo, ela não acumula.

Dois detalhes da linha 53 que podem ser perguntados:

- **não tem `\n`** porque o `fgets` já traz a quebra de linha grudada no fim. Se
  você pusesse outro, sairia uma linha em branco entre cada zumbi
- o **`"%s"`** está ali por segurança: escrever `fprintf(arquivo_log, linha)`
  direto faria o C tratar o conteúdo como instrução de formatação, e um `%` no
  meio bagunçaria tudo

**Linha 55 — gravar agora**

```c
55            fflush(arquivo_log);
```

Força o texto pro disco em vez de esperar acumular. É o que permite dar
`cat zumbie.txt` com o daemon rodando e já ver o conteúdo.

**Linha 56 — fechar o comando**

```c
56            pclose(comando);
```

Fecha o cano e **recolhe o processo filho** criado na linha 47.

> **Se perguntarem** *"se o teu programa cria um filho a cada ronda, por que
> esses filhos não viram zumbis também?"* — a resposta é esta linha. O `pclose`
> é o "recolher a fichinha" do filho. Sem ele, o daemon produziria um zumbi novo
> a cada 2 segundos: o programa que monitora zumbis seria a maior fábrica de
> zumbis da máquina.

E aí a chave fecha, o `while (1)` da linha 44 volta pro começo e tudo recomeça na
linha 46. Pra sempre, até chegar o `kill -TERM`.
