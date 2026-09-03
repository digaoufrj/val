# Lista 1 — Chamadas de Sistema (Prof. Valéria)

Exercícios de manipulação de arquivos em C usando chamadas de sistema do Linux
(`open`, `read`, `write`, `lseek`, `ftruncate`, `stat`, `fstat`, `lstat`).

Cada exercício tem sua própria pasta contendo:
- o código-fonte (`exN.c`),
- o binário já compilado (`exN`),
- o(s) arquivo(s) de teste que ele usa.

**Regra geral:** entre na pasta do exercício antes de compilar/rodar. Os arquivos
de teste são referenciados por caminho relativo (o ex4 e o ex8 dependem de estar
no diretório certo, porque o `link.txt` é um link simbólico relativo).

Compilação padrão:

```bash
gcc exN.c -o exN
```

---

## ex1 — Tamanho do arquivo com `lseek`

Abre o arquivo, pula para o fim com `lseek(fd, 0, SEEK_END)` e imprime a posição
(= tamanho em bytes).

```bash
cd ex1
gcc ex1.c -o ex1
./ex1 arquivo_teste.txt
```

Saída esperada:

```
O tamanho do arquivo 'arquivo_teste.txt' é: 39 bytes.
```

Recriar o arquivo de teste, se sumir:

```bash
echo "Testando o tamanho do arquivo no Linux" > arquivo_teste.txt
```

---

## ex2 — Ler 100 bytes do início e 50 bytes a partir do byte 200

Usa `read` + `lseek(fd, 200, SEEK_SET)`. Precisa de um arquivo com mais de 250
bytes — por isso o teste usa uma cópia do `/etc/passwd`.

```bash
cd ex2
gcc ex2.c -o ex2
./ex2 arquivo_grande.txt
```

Saída esperada (resumida):

```
--- Primeiros 100 bytes ---
root:x:0:0:root:/root:/bin/bash
...

--- 50 bytes a partir do byte 200 ---
0:games:/usr/games:/usr/sbin/nologin
man:x:6:12:ma
```

Recriar o arquivo de teste:

```bash
cat /etc/passwd > arquivo_grande.txt
```

> Obs.: o conteúdo do `/etc/passwd` varia de máquina para máquina, então o trecho
> impresso pode ser diferente do mostrado acima.

---

## ex3 — Metadados com `stat` (tamanho, links, UID, permissões, data)

```bash
cd ex3
gcc ex3.c -o ex3
./ex3 ex3_teste.txt
```

Saída esperada:

```
--- INFORMAÇÕES DO ARQUIVO ---
Tamanho: 37 bytes
Numero de links: 1
UID do dono: 1000
Permissoes de acesso: -rwxr-xr-x
Ultima modificacao: 02/09/2026 18:41:55
```

Recriar o arquivo de teste (o `chmod 755` é o que faz aparecer o `-rwxr-xr-x`):

```bash
echo "Testando a ficha criminal do arquivo" > ex3_teste.txt
chmod 755 ex3_teste.txt
```

---

## ex4 — `stat()` vs `lstat()` em link simbólico

O nome do arquivo é **fixo no código** (`link.txt`), então não recebe argumento.
Tem que rodar de dentro da pasta `ex4`, onde estão o `arquivo.txt` (alvo real) e
o `link.txt` (link simbólico apontando pra ele).

```bash
cd ex4
gcc ex4.c -o ex4
./ex4
```

Saída esperada:

```
Usando stat("link.txt"):
Tamanho retornado: 87 bytes      <- tamanho do arquivo REAL

Usando lstat("link.txt"):
Tamanho retornado: 11 bytes      <- tamanho do PRÓPRIO link ("arquivo.txt" = 11 chars)
```

Recriar os arquivos de teste:

```bash
echo "Este eh um arquivo real gigante cheio de coisas escritas dentro dele para ficar pesado" > arquivo.txt
ln -s arquivo.txt link.txt
```

---

## ex5 — `fstat()` vs `stat()` (tamanho + tipo do arquivo)

Funciona tanto com arquivo quanto com diretório.

```bash
cd ex5
gcc ex5.c -o ex5
./ex5 arquivo.txt     # -> 87 bytes, Arquivo Regular
./ex5 .               # -> 4096 bytes, Diretorio
```

Recriar o arquivo de teste:

```bash
echo "Este eh um arquivo real gigante cheio de coisas escritas dentro dele para ficar pesado" > arquivo.txt
```

---

## ex6 — Contar bytes nulos (`\0`) em arquivo binário

Percorre o arquivo byte a byte (`lseek` + `read` de 1 byte).

```bash
cd ex6
gcc ex6.c -o ex6
./ex6 binario.bin
```

Saída esperada:

```
O arquivo 'binario.bin' possui 3 bytes nulos (\0).
```

Recriar o arquivo binário de teste:

```bash
printf "AB\0CD\0EF\0G" > binario.bin
```

---

## ex7 — Escrever "FIM" no byte 20 e truncar o resto

⚠️ **Este programa MODIFICA o arquivo passado.** O `textao.txt` que está na pasta
já é o resultado depois de rodar (`01234567890123456789FIM`). Para ver o efeito
de novo, recrie o arquivo original antes:

```bash
cd ex7
gcc ex7.c -o ex7

# recria o arquivo original
echo "01234567890123456789A_PARTIR_DAQUI_TUDO_VAI_SER_APAGADO_E_CORTADO" > textao.txt
cat textao.txt      # antes

./ex7 textao.txt

cat textao.txt      # depois -> 01234567890123456789FIM
```

---

## ex8 — Informações de vários arquivos de uma vez (`lstat` em loop)

Aceita N argumentos e imprime tamanho, tipo e permissões de cada um. Usa `lstat`,
então detecta link simbólico corretamente. Rodar de dentro da pasta `ex8`.

```bash
cd ex8
gcc ex8.c -o ex8
./ex8 textao.txt link.txt .
```

Saída esperada:

```
--- Arquivo: textao.txt ---
Tamanho: 24 bytes
Tipo: Arquivo Regular
Permissoes: -rw-rw-r--

--- Arquivo: link.txt ---
Tamanho: 11 bytes
Tipo: Link Simbolico
Permissoes: lrwxrwxrwx

--- Arquivo: . ---
Tamanho: 4096 bytes
Tipo: Diretorio
Permissoes: drwxrwxr-x
```

Recriar os arquivos de teste:

```bash
echo "01234567890123456789FIM" > textao.txt
echo "Este eh um arquivo real gigante cheio de coisas escritas dentro dele para ficar pesado" > arquivo.txt
ln -s arquivo.txt link.txt
```

---

## Recompilar tudo de uma vez

Da pasta `Lista 1`:

```bash
for d in ex*/; do (cd "$d" && gcc "${d%/}.c" -o "${d%/}"); done
```

## Tabela rápida

| Ex | Tema | Chamadas | Como rodar |
|----|------|----------|------------|
| 1 | Tamanho do arquivo | `open`, `lseek` | `./ex1 arquivo_teste.txt` |
| 2 | Leitura com deslocamento | `read`, `lseek` | `./ex2 arquivo_grande.txt` |
| 3 | Metadados completos | `stat` | `./ex3 ex3_teste.txt` |
| 4 | Link simbólico | `stat` vs `lstat` | `./ex4` (sem argumento) |
| 5 | Descritor vs caminho | `fstat` vs `stat` | `./ex5 arquivo.txt` / `./ex5 .` |
| 6 | Contar bytes nulos | `lseek`, `read` | `./ex6 binario.bin` |
| 7 | Escrita + truncamento | `write`, `ftruncate` | `./ex7 textao.txt` (destrutivo) |
| 8 | Vários arquivos | `lstat` em loop | `./ex8 textao.txt link.txt .` |
