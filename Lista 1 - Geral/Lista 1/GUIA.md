# Guia — conceitos e como explicar na hora

O código de [`Lista 1 - refactor`](../Lista%201%20-%20refactor/) tem pouquíssimo
comentário de propósito. **O que estava escrito lá dentro está aqui**, na seção
"o que cada trecho faz" de cada exercício.

---

## Os 4 instrumentos

| | O que é | Recebe | Devolve |
|---|---|---|---|
| `lseek(fd, off, origem)` | move o **cursor** do arquivo | descritor | **a nova posição** (é isso que dá o truque do tamanho) |
| `stat(nome, &buf)` | ficha do arquivo | **nome** (string) | preenche `struct stat`; **segue** link simbólico |
| `lstat(nome, &buf)` | idem | **nome** | igual, mas **não segue** o link — fala do link em si |
| `fstat(fd, &buf)` | idem | **descritor** (já aberto) | mesma `struct stat` |

**A ideia central:** os três `stat` dão a **mesma informação**, mudando só *como você aponta pro arquivo* — por nome (`stat`), por nome sem seguir link (`lstat`), ou por arquivo já aberto (`fstat`).

**Origens do `lseek`:** `SEEK_SET` = absoluto (do início), `SEEK_CUR` = relativo ao cursor, `SEEK_END` = a partir do fim.

**`st_mode` guarda duas coisas no mesmo número:** o tipo e as permissões.
- Tipo → macros: `S_ISREG`, `S_ISDIR`, `S_ISLNK`
- Permissões → máscara com **`&`**: `st_mode & S_IRUSR`

**`struct stat` é um formulário em branco** que eu declaro e passo por endereço
(`&info`); quem preenche é o kernel.

---

## ex1 — tamanho com `lseek`

**Conceito:** `lseek` devolve a nova posição do cursor.

*"Abro o arquivo, mando o cursor pro fim com `SEEK_END` e deslocamento 0. Como o `lseek` retorna a posição onde parou, e ele parou no fim, esse número é o tamanho em bytes."*

**O que cada trecho faz:**
- `if (argc != 2)` — confere se o nome do arquivo veio na linha de comando
- `open(argv[1], O_RDONLY)` — abre só para leitura; devolve o descritor
- `lseek(fd, 0, SEEK_END)` — vai pro fim e **retorna a posição**, que é o tamanho
- `close(fd)` — devolve o descritor pro sistema

---

## ex2 — ler pedaços

**Conceito:** o `read` **avança o cursor sozinho**; o `lseek` reposiciona.

*"Leio 100 bytes, o cursor já fica no 100. Aí uso `lseek(fd, 200, SEEK_SET)` — absoluto, do início — pra pular pro byte 200 e leio mais 50."*

**O que cada trecho faz:**
- `char buf[101] = {0}` — **101 e não 100**: o espaço a mais é pro `\0`. O `read` **não coloca terminador**, e sem ele o `printf("%s")` sairia imprimindo lixo da memória. O `= {0}` zera tudo, então o terminador já está lá
- `read(fd, buf, 100)` — lê 100 bytes e devolve quantos leu de fato
- `lseek(fd, 200, SEEK_SET)` — `SEEK_SET` conta do **início**, então cai no byte 200 independente de onde o cursor estava
- o `else` do segundo `read` cobre arquivo pequeno demais

---

## ex3 — ficha do arquivo com `stat`

**Conceito:** `struct stat` + `st_mode` como bitmask.

*"O `stat` preenche a struct. Tamanho, links e UID são campos diretos. As permissões estão bit a bit dentro do `st_mode`, então testo cada uma com `&`: se `st_mode & S_IRUSR` der diferente de zero, o bit de leitura do dono está ligado."*

**O que cada trecho faz:**
- `struct stat info;` — o formulário em branco que o `stat` vai preencher
- `stat(argv[1], &info)` — passo o **endereço** pra função poder escrever nele
- `st_size`, `st_nlink`, `st_uid` — campos diretos da struct
- as 10 linhas de `printf` — primeiro o caractere de tipo (`d` ou `-`), depois os 9 bits `rwx` de dono/grupo/outros, montando o formato do `ls -l`
- `localtime(&info.st_mtime)` — o `st_mtime` é o número de **segundos desde 1970**; o `localtime` quebra isso em dia/mês/ano/hora
- `strftime(...)` — monta a string no formato `%d/%m/%Y %H:%M:%S`

---

## ex4 — `stat` vs `lstat`  ← *o enunciado pede pra explicar a diferença*

**Conceito:** seguir ou não o link.

*"Um link simbólico é um arquivo que guarda o caminho do alvo como texto. O `stat` segue o link e me dá os 87 bytes do arquivo real. O `lstat` para no link e me dá 11 bytes — que é exatamente o tamanho da string `arquivo.txt`, 11 caracteres."*

**O que cada trecho faz:**
- `const char *nome_link = "link.txt"` — o nome é fixo porque **o enunciado já diz qual é o link** (`ln -s arquivo.txt link.txt`)
- duas `struct stat` separadas — pra poder mostrar os dois resultados lado a lado
- o `perror` avisa se o link ou o arquivo original não existirem

---

## ex5 — `fstat` vs `stat`

**Conceito:** mesma informação, jeito diferente de apontar.

*"O `fstat` precisa do descritor, então tenho que abrir o arquivo antes. O `stat` só precisa do nome. Rodei os dois no mesmo arquivo e o tamanho e o tipo deram idênticos — é a mesma struct, só muda a forma de chegar nela."*

**O que cada trecho faz:**
- `mostra_tipo()` — função separada só pra não repetir o mesmo `if/else` duas vezes
- `open()` antes do `fstat` — **obrigatório**: o `fstat` só aceita descritor, não aceita nome
- `close(fd)` logo depois do `fstat` — o descritor já cumpriu o papel dele
- `stat(argv[1], ...)` — esse não precisa de `open` nenhum, trabalha direto com o nome

Testei também com diretório: `./ex5 .` → 4096 bytes, Diretório.

---

## ex6 — contar bytes nulos

**Conceito:** percorrer o arquivo com `lseek` posição a posição.

*"Pego o tamanho com `SEEK_END`, volto e, num laço, uso `lseek(fd, i, SEEK_SET)` pra ir em cada posição e ler 1 byte, contando os `\0`."*

**O que cada trecho faz:**
- `lseek(fd, 0, SEEK_END)` — mesmo truque do ex1, aqui só pra saber até onde o laço vai
- `lseek(fd, i, SEEK_SET)` dentro do laço — pula **exatamente** pra posição `i`
- `read(fd, &b, 1)` — lê **um único byte** daquela posição
- `if (b == '\0')` — `\0` é o byte de valor zero, comum em arquivo binário e raro em texto

⚠️ Se ela questionar eficiência: dá pra ler sequencialmente sem `lseek` a cada byte. **Mas o enunciado diz "use `lseek()` para percorrê-lo"**, então eu segui a letra do enunciado.

---

## ex7 — escrever no meio e truncar

**Conceito:** escrever **não** apaga o resto; quem corta é o `ftruncate`.

*"Posiciono no byte 20, escrevo `FIM\n` — isso sobrescreve 4 bytes, mas o resto do arquivo continuaria lá. Por isso pego a posição atual depois da escrita e chamo `ftruncate` nela, que corta tudo dali pra frente. O arquivo vai de 66 pra 24 bytes."*

**O que cada trecho faz:**
- `O_WRONLY` — só escrita, é tudo que o exercício precisa
- `lseek(fd, 20, SEEK_SET)` — estaciona o cursor no byte 20
- `write(fd, "FIM\n", 4)` — **4 na mão, não `sizeof`**: `sizeof("FIM\n")` vale **5** porque conta o `\0` que fecha toda string em C, e esse byte nulo iria parar dentro do arquivo
- `lseek(fd, 0, SEEK_CUR)` — deslocamento 0 a partir do cursor: não move nada, só **pergunta** onde ele está (24, depois dos 4 bytes escritos)
- `ftruncate(fd, pos)` — corta o arquivo nesse tamanho, jogando fora o resto

---

## ex8 — vários arquivos

**Conceito:** laço no `argv` + `lstat`.

*"Começo o laço no `argv[1]` pra pular o nome do programa. Uso **`lstat` e não `stat`** de propósito: com `stat`, o link simbólico apareceria como se fosse o arquivo alvo e eu nunca conseguiria mostrar o tipo 'link'."*

**O que cada trecho faz:**
- `#include <sys/types.h>` — é ele que traz o tipo `mode_t`, usado nos parâmetros das duas funções auxiliares
- `for (int i = 1; ...)` — **começa em 1** porque `argv[0]` é o nome do próprio programa
- `lstat` — a escolha central do exercício, como explicado acima
- `continue` no erro — avisa e segue pros próximos arquivos em vez de abortar tudo
- `mostra_permissoes()` — o primeiro caractere é o tipo (`d`/`l`/`-`), depois os 9 bits, igual ao `ls -l`

---

## Perguntas que ela pode fazer

- **"O que é o `fd`?"** — Um número inteiro, índice na tabela de arquivos abertos do processo. 0/1/2 já são stdin/stdout/stderr, por isso o primeiro `open` costuma devolver 3.
- **"Por que `off_t` e não `int`?"** — `off_t` é 64 bits; `int` estoura em arquivo acima de 2 GB.
- **"Por que o diretório tem 4096 bytes?"** — É o tamanho do bloco que o filesystem reserva pra guardar a lista de nomes do diretório, não a soma do conteúdo.
- **"Qual a diferença de `read` pra `lseek`?"** — `read` copia dados **e** move o cursor; `lseek` só move o cursor, não lê nada.
- **"E se o `open` falhar?"** — Retorna -1. Todos os programas checam e chamam `perror`, que imprime a mensagem do erro que está em `errno`.
- **"Por que passa `&info` e não `info`?"** — A função precisa **escrever** na struct. Em C só dá pra alterar a variável de quem chamou passando o endereço dela.
