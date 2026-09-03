# Conferência: versão do Robson x minha versão

Compilei tudo com `gcc -Wall -Wextra` e rodei os dois lados nos mesmos arquivos
de teste. Resumo: **os 8 exercícios têm o mesmo sentido e 5 estão corretos.
1 tem bug de lógica, 2 têm defeitos menores, e 1 está incompleto.**

| Ex | Tema | Resultado |
|----|------|-----------|
| 1 | tamanho via `lseek` | ✅ correto |
| 2 | ler 100 bytes + 50 a partir do byte 200 | ⚠️ funciona, mas imprime lixo no fim |
| 3 | metadados via `stat` | ❌ **bug: permissões sempre erradas** |
| 4 | `stat` vs `lstat` em link | ✅ correto |
| 5 | `fstat` vs `stat` | ⚠️ **incompleto: nunca chama `stat()`** |
| 6 | contar bytes nulos | ✅ correto (e mais eficiente que o meu) |
| 7 | escrever "FIM" no byte 20 e truncar | ⚠️ grava 1 byte a mais (`\0`) |
| 8 | vários arquivos via `lstat` | ✅ lógica correta, saída mais enxuta |

---

## ❌ e3 — permissões sempre mostram `rwx`

O erro está no operador vírgula no lugar do `&`:

```c
printf((file_info.st_mode, S_IRUSR)? "r":"-");   // errado
printf((file_info.st_mode & S_IRUSR)? "r":"-");  // certo (o que eu usei)
```

`(a, b)` avalia `a`, **joga fora**, e usa `b`. Como `S_IRUSR` é a constante `0400`,
a condição é sempre verdadeira → imprime `rwx` para qualquer arquivo. O próprio
gcc avisa: `warning: left-hand operand of comma expression has no effect`.

Teste que comprova, num arquivo `chmod 400`:

```
$ ls -l somenteleitura.txt
-r-------- 1 rodrigo rodrigo 22 somenteleitura.txt   <- real: só leitura

$ ./e3
Digite o nome do arquivo: somenteleitura.txt
Tamanho:22
Numero de links:1
UID:1000
rwx                                                   <- errado, deveria ser r--
```

Detalhe secundário: ele imprime só as 3 permissões do **dono**. O enunciado pede
as permissões de acesso, que normalmente são as 9 (dono/grupo/outros) — o meu
`ex3` imprime as 10 posições no formato do `ls -l`.

Curiosidade: **no `e8` ele escreveu certo** (`info.st_mode & S_IRUSR`). O erro do
e3 não se repetiu.

---

## ⚠️ e5 — falta a metade do exercício

O exercício é **comparar `fstat()` com `stat()`**. O `e5.c` só usa `fstat()` —
a chamada `stat()` não aparece no arquivo. No lugar da comparação ele fez uma
detecção de link simbólico com `O_NOFOLLOW` + `errno == ELOOP`, que é uma ideia
válida e até elegante, mas responde outra pergunta.

Também não imprime o **tamanho** (só o tipo), e o nome do arquivo é fixo em
`lorem.txt`, então não dá pra testar com diretório como o meu (`./ex5 .`).

O que ele tem funciona e o resultado está certo — é escopo faltando, não erro.

---

## ⚠️ e7 — grava um byte nulo extra no arquivo

```c
char frase[] = "FIM\n";
write(file, frase, sizeof(frase));   // sizeof = 5, inclui o '\0' final
```

`sizeof("FIM\n")` é **5**, não 4: conta o terminador de string. Então o `\0` vai
parar dentro do arquivo, e o `ftruncate` corta depois dele.

Comparação nos mesmos 66 bytes de entrada:

```
meu ex7  -> 24 bytes:  30 31 ... 39 46 49 4d 0a         "...789FIM\n"
e7 dele  -> 25 bytes:  30 31 ... 39 46 49 4d 0a 00      "...789FIM\n\0"
```

Visualmente o `cat` mostra a mesma coisa (o `\0` é invisível), mas o arquivo tem
1 byte a mais. O certo é `write(file, frase, 4)` ou `strlen(frase)`.

---

## ⚠️ e2 — buffers sem terminador, imprime lixo

Ele lê com `read()` e imprime direto com `%s`, mas `read()` **não coloca `\0`** no
fim. Os buffers (`texto[200]`, `texto2[100]`) não são inicializados, então o
`printf` continua imprimindo o que estiver na memória depois dos bytes lidos.

Saída real com `cat -A` (o `^A` é lixo, byte `0x01`, não faz parte do arquivo):

```
Texto ->0:games:/usr/games:/usr/sbin/nologin
man:x:6:12:ma^A
Quantidade lida:50
```

Na primeira leitura não apareceu lixo por sorte (a pilha estava zerada), mas é
comportamento indefinido — pode mudar de máquina para máquina. A correção é
`char texto[200] = {0};` (foi o que eu fiz) ou pôr `texto[lidos] = '\0'`.

**O deslocamento está certo**, por um caminho diferente do meu:

```c
lseek(cursor, 100, SEEK_CUR);   // ele: já estava no 100 após o read, 100+100 = 200
lseek(fd, 200, SEEK_SET);       // eu: pula direto para 200
```

Os dois param no byte 200 e leem o mesmo trecho — confirmei que a saída bate.

---

## ✅ e6 — correto, e melhor que o meu

Mesmo resultado do meu `ex6` (testei nos dois com `printf "AB\0CD\0EF\0G"`:
ambos dizem 3 nulos; no binário de 16 KB dele deu 13485, que confere com uma
contagem independente via `od`).

A implementação dele é **mais eficiente**: lê sequencialmente 1 byte por vez,
enquanto o meu faz `lseek(fd, i, SEEK_SET)` antes de cada `read` — um syscall a
mais por byte, sem necessidade, já que o cursor avança sozinho.

---

## ✅ e1, e4, e8 — corretos

- **e1**: mesmo `lseek(fd, 0, SEEK_END)`. Testei nos dois com o mesmo arquivo,
  resultado idêntico. Ele não recebe argumento e não checa erro do `open` (se o
  arquivo não existir, `lseek` retorna -1 e imprime `-1` em vez de mensagem).
- **e4**: idêntico em resultado ao meu — `stat` devolve o tamanho do alvo,
  `lstat` o tamanho do link. Confirmado: 28 vs 9 bytes no meu teste.
- **e8**: usa `lstat` em loop no `argv` e identifica regular/link/diretório
  corretamente, com o `&` certo nas permissões. Diferenças cosméticas: imprime
  só as 3 permissões do dono, não mostra o caractere de tipo (`d`/`l`/`-`), não
  repete o nome do arquivo (só o índice) e aborta com `return 1` no primeiro
  arquivo inválido, enquanto o meu usa `continue` e segue para os próximos.

---

## Observações gerais no código dele

Nada disso é erro de exercício, mas apareceu nos testes:

1. **Nenhum `close()`** em e1, e4, e5, e6 e e7 (o e2 e o e3 fecham). Na prática o
   sistema fecha ao encerrar o processo, mas é boa prática fechar.
2. **`open()` sem checar retorno** em e1, e2, e6 e e7 — se o arquivo não existir,
   o programa segue com `fd = -1` e falha silenciosamente.
3. **Nomes de arquivo fixos no código** em quase todos, o que dificulta testar
   casos diferentes (o enunciado do e8 já pedia argumentos, e ele fez com argv).
4. **`int` no lugar de `off_t`** para tamanho de arquivo (e1, e6) — quebra em
   arquivos acima de 2 GB.
5. `puts("\n")` no e8 imprime duas quebras de linha (o `puts` já adiciona uma).
