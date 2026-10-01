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

## 6. `lista2_ajustado.c` — o que cada trecho faz

### Antes do `main`

- `FILE *arquivo_log;` — está **fora** de qualquer função (variável global) de
  propósito: a função que trata o sinal também precisa enxergar o arquivo
- `void trata_sinal(int sig)` — o *handler*. Esta função **não é chamada por
  você**; o sistema a dispara sozinho quando o SIGTERM chega. Ela escreve a
  despedida, fecha o arquivo e encerra com `exit(0)`

### Preparação

- `if ((n = ...) <= 0)` — mesma validação do outro programa
- `fopen("zumbie.txt", "a+")` — abre o log. O **`a`** é de *append*: escreve
  **no fim**, sem apagar o que já tinha
- `if (arquivo_log == NULL) { perror(...); return 1; }` — se não conseguiu abrir
  (pasta sem permissão, por exemplo), avisa e sai. Sem isso o programa quebraria
  feio na hora de escrever
- `if (fork()) exit(0);` — **vira daemon aqui**, mesmo truque do outro programa:
  o pai morre, o filho segue em background adotado pelo sistema

### Os sinais

```c
for (i = 1; i < NSIG; i++)
    if (i != SIGCHLD)
        (void)signal(i, SIG_IGN);
(void)signal(SIGTERM, trata_sinal);
```

- a linha do `for` passa por **todos os sinais que existem** (`NSIG` é a
  quantidade total) e manda **ignorar** cada um — é assim que ele fica
  invulnerável, como o enunciado pede
- o `if (i != SIGCHLD)` deixa de fora o "um filho seu morreu", que é o aviso
  usado pelo `pclose` pra saber que o `ps` daquela rodada terminou. (Testando,
  funciona dos dois jeitos; deixar o SIGCHLD em paz é só o mais correto)
- `SIGKILL` e `SIGSTOP` o sistema simplesmente não deixa ignorar. Não dá erro,
  ele só não obedece — e está certo, o enunciado já avisa que o SIGKILL mata
- a última linha registra o handler **depois** do laço, **por cima** do ignorar.
  Por isso o SIGTERM volta a funcionar e é o único jeito educado de encerrar
- o `(void)` na frente só diz "eu sei que esta função devolve algo e estou
  ignorando de propósito"

### O laço principal

```c
while (1)
{
    sleep(n);
    FILE *comando = popen("ps -eo pid,ppid,comm,stat --no-headers | awk '$4 ~ /^Z/ {print $1, $2, $3}'", "r");
```

- `while (1)` — para sempre; quem interrompe é o SIGTERM
- `sleep(n)` — dorme os `n` segundos. É isso que faz ele "acordar de n em n"
- `popen(..., "r")` — roda aquele comando **como processo filho** e devolve um
  "arquivo" pra ler a saída dele. O `"r"` é de *read*

O comando lá dentro, em pedaços:

| Pedaço | O que faz |
|---|---|
| `ps -e` | lista **todos** os processos |
| `-o pid,ppid,comm,stat` | escolhe as colunas: PID, PID do pai, nome, estado |
| `--no-headers` | tira a linha de título, que atrapalharia |
| `awk '$4 ~ /^Z/'` | fica só com as linhas cuja **4ª coluna** (o estado) **começa com Z** — ou seja, só os zumbis |
| `{print $1, $2, $3}` | imprime as três primeiras colunas, na ordem que o PDF pede |

- `fprintf(arquivo_log, "=====...")` — a linha separadora, uma por rodada
- `while (fgets(linha, sizeof(linha), comando) != NULL)` — lê a saída do `ps`
  **linha por linha** até acabar. O `sizeof(linha)` evita escrever além do
  tamanho do vetor
- `fflush(arquivo_log)` — **força a gravação agora**. Sem isso o texto fica
  esperando num buffer na memória e você abriria o log e veria vazio
- `pclose(comando)` — fecha o cano e libera o processo do `ps`. Está **dentro**
  do `if` porque só faz sentido fechar o que abriu

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
