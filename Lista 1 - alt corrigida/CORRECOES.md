# O que foi corrigido, linha a linha

Cada item abaixo é uma diferença entre [`Lista 1 - alt`](../Lista%201%20-%20alt/)
(original do Robson) e esta pasta. O código aqui não tem comentário nenhum além
dos dois que já eram dele no `e1.c` — a explicação toda mora neste arquivo.

Para ver as diferenças direto no terminal:

```bash
diff -u "../Lista 1 - alt/e3/e3.c" e3/e3.c
```

---

## e1 — leve

| Era | Ficou | Por quê |
|---|---|---|
| `signed int tam_bytes` | `off_t tam_bytes` | `int` é 32 bits e estoura em arquivo acima de 2 GB; `off_t` é o tipo próprio para deslocamento de arquivo |
| sem checagem do `open` | `if(cursor == -1)` | sem isso, arquivo inexistente dava `cursor = -1`, o `lseek` falhava e o programa imprimia `-1` como se fosse o tamanho |
| sem checagem do `lseek` | `if(tam_bytes == -1)` | mesma ideia |
| `printf("%d", ...)` | `printf("Tamanho: %ld bytes\n", ...)` | faltava a quebra de linha, e `%ld` casa com o `(long)` do cast |
| sem `close` | `close(cursor)` | o descritor ficava aberto até o processo morrer |

## e2 — média

| Era | Ficou | Por quê |
|---|---|---|
| `char name[10]` | `char name[256]` | só cabia nome de 9 letras; `lorem.txt` tem exatamente 9 e funcionava por pouco |
| sem `strcspn` | `name[strcspn(name,"\n")] = '\0'` | o `fgets` guarda o Enter no fim da string, e o `open` procurava um arquivo com `\n` no nome |
| `char texto[200]` | `char texto[200] = {0}` + `texto[tam] = '\0'` | **o `read()` não coloca terminador de string.** O `printf("%s")` continuava imprimindo a memória depois dos bytes lidos |
| `char texto2[100]` | idem | era aqui que aparecia o lixo visível: um byte `0x01` colado no fim do texto |
| sem checagem do `open`/`read` | `if(... == -1)` | falha silenciosa |
| `lseek(cursor,100,SEEK_CUR)` | `lseek(cursor,200,SEEK_SET)` | dava no mesmo (100 já lidos + 100 = 200), mas **por sorte**: se o `read` devolvesse menos de 100 bytes, o salto erraria o alvo. `SEEK_SET` é absoluto e é o que o enunciado pede |

## e3 — **grave**

O bug principal da lista inteira:

```c
printf((file_info.st_mode, S_IRUSR)? "r":"-");   // era assim
printf((file_info.st_mode & S_IRUSR)? "r":"-");  // ficou assim
```

Com **vírgula**, o C avalia o lado esquerdo, **descarta**, e usa só o lado
direito. Como `S_IRUSR` é a constante `0400`, a condição era sempre verdadeira e
o programa imprimia `rwx` para qualquer arquivo. O gcc já avisava:
`warning: left-hand operand of comma expression has no effect`.

O certo é o **E bit a bit** (`&`), que testa se aquele bit está ligado dentro do
`st_mode`.

Prova, num arquivo `chmod 400`:

```
$ ls -l t.txt
-r-------- 1 rodrigo rodrigo 11 t.txt

original:  rwx          <- errado
corrigido: r--------    <- bate com o ls
```

Também foram acrescentadas as permissões de **grupo e outros** (`S_IRGRP`,
`S_IWGRP`, `S_IXGRP`, `S_IROTH`, `S_IWOTH`, `S_IXOTH`) — o enunciado pede as
permissões de acesso, que são as 9, e ele imprimia só as 3 do dono. E o
`char name[20]` virou `[256]`.

## e4 — média

**Nomes alinhados com o enunciado.** O enunciado dá os nomes de propósito
(*"Crie um link simbólico para um arquivo (`ln -s arquivo.txt link.txt`)"*, e
depois *"Use `stat("link.txt")`"*). O original usava `link` → `lorem.txt`.
Agora a pasta tem `arquivo.txt` e `link.txt`, exatamente como no enunciado, e o
`lstat` devolve **11 bytes** — as 11 letras de `arquivo.txt`, o número que a
própria professora usa de exemplo.

E o enunciado termina com **"Explique a diferença"**, que o programa não fazia:
só imprimia os dois números. Foram acrescentados os `printf` que explicam:

- `stat()` **segue** o link → devolve o tamanho do arquivo alvo (634 bytes)
- `lstat()` **não segue** → devolve o tamanho do próprio link (9 bytes)
- e o porquê: o link é um arquivo que guarda o caminho do alvo como texto, então
  seu tamanho é o número de letras desse caminho — `arquivo.txt` tem 11 letras

## e5 — **grave**

**A chamada `stat()` não existia no arquivo.** O enunciado pede *"Compare com
`stat()` para verificar que o resultado é o mesmo"*, e o programa só usava
`fstat()`. Foi acrescentado:

- a chamada `stat(nome, &info2)`
- a impressão do **tamanho**, que também faltava (só mostrava o tipo)
- uma comparação explícita no fim: `Os dois resultados sao iguais? SIM`
- o teste `S_ISLNK`, já que o enunciado pede regular / diretório / **link**
- a função `mostra_tipo()`, só para não repetir o mesmo `if/else` duas vezes

A detecção de link por `O_NOFOLLOW` + `errno == ELOOP` que ele tinha inventado
**foi mantida** — é uma solução válida e engenhosa, só não era o que o exercício
pedia.

Além disso o nome do arquivo saiu de fixo (`"lorem.txt"`) para argumento
opcional, o que permite testar com diretório: `./e5 .`

## e6 — média

**O ponto principal: quem percorria o arquivo era o `read`, não o `lseek`.**
O enunciado pede *"Use `lseek()` para percorrê-lo sem `read()` tradicional"*.
No original o `lseek` aparecia só duas vezes — uma pra medir o tamanho e uma pra
voltar ao início — e a varredura era um `read` sequencial. Se a professora
perguntasse "onde você percorreu com `lseek`?", não havia resposta.

```c
lseek(cursor,0,SEEK_SET);      // era: rebobina uma vez
while(i < tamanho)
{
    read(cursor,&letra,1);     // e o read anda sozinho
```

```c
while(i < tamanho)
{
    lseek(cursor,i,SEEK_SET);  // ficou: salta pra posicao i a cada volta
    read(cursor,&letra,1);
```

O resultado é o mesmo (13485 bytes nulos no `binario.bin`, conferido por fora
com `od`), mas agora é o `lseek` que percorre, que é o que o exercício cobra.
Fica mais lento — um syscall a mais por byte — e essa é a resposta honesta se
ela cutucar: dá pra fazer sequencial, mas o enunciado pediu `lseek`.

Além disso:

| Era | Ficou | Por quê |
|---|---|---|
| `open("e5", ...)` | argumento opcional, padrão `binario.bin` | estava preso ao nome do binário que por acaso estava na pasta |
| `int tamanho` | `off_t tamanho` | mesmo motivo do e1 |
| sem checagem do `open` | `if(cursor == -1)` | falha silenciosa |
| sem `close` | `close(cursor)` | descritor vazando |

A lógica de contagem em si já estava certa desde o começo — o que mudou foi
**como** o arquivo é percorrido.

## e7 — média

```c
write(file, frase, sizeof(frase));   // era assim
write(file, frase, strlen(frase));   // ficou assim
```

**`sizeof("FIM\n")` vale 5, não 4** — conta o `\0` que fecha toda string em C.
Esse byte nulo ia parar dentro do arquivo, e o `ftruncate` cortava depois dele:

```
original:  25 bytes -> 30 31 ... 39 46 49 4d 0a 00   ("...789FIM\n\0")
corrigido: 24 bytes -> 30 31 ... 39 46 49 4d 0a      ("...789FIM\n")
```

O `strlen` conta só os caracteres visíveis, sem o terminador.

Também: `SEEK_CUR` → `SEEK_SET` no salto para o byte 20 (dava no mesmo porque o
arquivo tinha acabado de abrir com o cursor em 0, mas `SEEK_SET` diz exatamente
o que o enunciado pede), checagem do `open`, `close` no fim, e
`buffer[lidos] = '\0'` antes do `printf("%s")` — de novo o `read` que não
termina string.

## e8 — média

| Era | Ficou | Por quê |
|---|---|---|
| `int file;` | removido | declarada e nunca usada (o gcc avisava) |
| nada quando `argc < 2` | mensagem de uso | o programa terminava sem dizer nada |
| `return 1` no erro do `lstat` | `perror` + `continue` | abortava tudo no primeiro arquivo inválido em vez de seguir para os próximos |
| `"Tamanho texto %d"` com índice | `--- %s ---` com o nome | o índice não diz de qual arquivo é |
| só `rwx` do dono | 9 permissões + caractere de tipo | igual ao formato do `ls -l`: `-rw-rw-r--`, `lrwxrwxrwx`, `drwxrwxr-x` |
| sem `else` no tipo | `else printf("outro\n")` | socket, device etc. não imprimiam nada |
| `puts("\n")` | `printf("\n\n")` | o `puts` já adiciona um `\n`, então saíam duas linhas em branco |

O uso de `lstat` em vez de `stat` já estava certo, e é o ponto principal do
exercício: com `stat` o link simbólico apareceria como o arquivo alvo e nunca
daria para mostrar o tipo "link".
