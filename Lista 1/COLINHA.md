# Colinha — conceitos e como explicar na hora

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

---

## Exercício por exercício

**ex1 — tamanho com `lseek`**
Conceito: `lseek` devolve a nova posição do cursor.
*"Abro o arquivo, mando o cursor pro fim com `SEEK_END` e deslocamento 0. Como o `lseek` retorna a posição onde parou, e ele parou no fim, esse número é o tamanho em bytes."*

**ex2 — ler pedaços**
Conceito: o `read` **avança o cursor sozinho**; o `lseek` reposiciona.
*"Leio 100 bytes, o cursor já fica no 100. Aí uso `lseek(fd, 200, SEEK_SET)` — absoluto, do início — pra pular pro byte 200 e leio mais 50."*
⚠️ Se ela perguntar do buffer: `read` **não coloca `\0`**, por isso declarei `char buffer[101] = {0}` — o espaço a mais é o terminador.

**ex3 — ficha do arquivo com `stat`**
Conceito: `struct stat` + `st_mode` como bitmask.
*"O `stat` preenche a struct. Tamanho, links e UID são campos diretos. As permissões estão bit a bit dentro do `st_mode`, então testo cada uma com `&`: se `st_mode & S_IRUSR` der diferente de zero, o bit de leitura do dono está ligado."*
Data: `localtime` transforma o `st_mtime` (segundos desde 1970) em `struct tm`, e o `strftime` formata.

**ex4 — `stat` vs `lstat`** ← *o enunciado pede pra explicar a diferença*
Conceito: seguir ou não o link.
*"Um link simbólico é um arquivo que guarda o caminho do alvo como texto. O `stat` segue o link e me dá os 87 bytes do arquivo real. O `lstat` para no link e me dá 11 bytes — que é exatamente o tamanho da string `arquivo.txt`, 11 caracteres."*

**ex5 — `fstat` vs `stat`**
Conceito: mesma informação, jeito diferente de apontar.
*"O `fstat` precisa do descritor, então tenho que abrir o arquivo antes. O `stat` só precisa do nome. Rodei os dois no mesmo arquivo e o tamanho e o tipo deram idênticos — é a mesma struct, só muda a forma de chegar nela."*
Testei com diretório também (`./ex5 .` → 4096 bytes, Diretório).

**ex6 — contar bytes nulos**
Conceito: percorrer com `lseek` posição a posição.
*"Pego o tamanho com `SEEK_END`, volto e, num laço, uso `lseek(fd, i, SEEK_SET)` pra ir em cada posição e ler 1 byte, contando os `\0`."*
⚠️ Se ela questionar eficiência: dá pra ler sequencialmente sem `lseek` a cada byte. **Mas o enunciado diz "use `lseek()` para percorrê-lo"**, então eu segui a letra do enunciado.

**ex7 — escrever no meio e truncar**
Conceito: escrever **não** apaga o resto; quem corta é o `ftruncate`.
*"Posiciono no byte 20, escrevo `FIM\n` — isso sobrescreve 4 bytes, mas o resto do arquivo continuaria lá. Por isso pego a posição atual depois da escrita e chamo `ftruncate` nela, que corta tudo dali pra frente. O arquivo vai de 66 pra 24 bytes."*
⚠️ Escrevi `write(fd, "FIM\n", 4)` com **4** na mão, não `sizeof`: `sizeof("FIM\n")` é **5**, porque conta o `\0` do fim da string, e esse nulo iria parar dentro do arquivo.

**ex8 — vários arquivos**
Conceito: laço no `argv` + `lstat`.
*"Começo o laço no `argv[1]` pra pular o nome do programa. Uso **`lstat` e não `stat`** de propósito: com `stat`, o link simbólico apareceria como se fosse o arquivo alvo e eu nunca conseguiria mostrar o tipo 'link'."*
Uso `continue` no erro pra não abortar os outros arquivos.

---

## Perguntas que ela pode fazer

- **"O que é o `fd`?"** — Um número inteiro, índice na tabela de arquivos abertos do processo. 0/1/2 já são stdin/stdout/stderr, por isso o primeiro `open` costuma devolver 3.
- **"Por que `off_t` e não `int`?"** — `off_t` é 64 bits; `int` estoura em arquivo acima de 2 GB.
- **"Por que o diretório tem 4096 bytes?"** — É o tamanho do bloco que o filesystem reserva pra guardar a lista de nomes do diretório, não a soma do conteúdo.
- **"Qual a diferença de `read` pra `lseek`?"** — `read` copia dados **e** move o cursor; `lseek` só move o cursor, não lê nada.
- **"E se o `open` falhar?"** — Retorna -1. Todos os meus programas checam e chamam `perror`, que imprime a mensagem do erro que está em `errno`.
