# Lista 1 — versão do Robson, corrigida

Mesmo código do [Robson](https://github.com/RobsDuarte/Learning-Linux), com os
defeitos consertados. Os originais continuam intactos em
[`Lista 1 - alt`](../Lista%201%20-%20alt/) para comparação lado a lado.

Cada correção está marcada com um comentário `// CORRIGIDO:` no próprio código,
explicando o que estava errado e por quê. A estrutura, o estilo e os nomes de
variável dele foram mantidos — só o que estava quebrado mudou.

Compila sem nenhum aviso com `gcc -Wall -Wextra`.

## O que mudou em cada um

| Ex | Gravidade | Correção principal |
|----|-----------|--------------------|
| e1 | leve | `off_t` no lugar de `int`, checagem do `open`, `close`, `\n` no print |
| e2 | média | buffers zerados e terminados (imprimia lixo), nome com 256 bytes e sem `\n`, `SEEK_SET 200` |
| e3 | **grave** | `,` → `&` nas permissões (imprimia `rwx` sempre), + grupo e outros |
| e4 | média | adicionada a explicação da diferença, que o enunciado pede |
| e5 | **grave** | adicionada a chamada `stat()` e a comparação com `fstat()`, que faltavam |
| e6 | leve | aceita argumento, `off_t`, checagem do `open`, `close` |
| e7 | média | `strlen` no lugar de `sizeof` (gravava um `\0` extra no arquivo) |
| e8 | média | permissões completas (9) + caractere de tipo, mostra o nome, segue após erro |

## Como rodar

```bash
cd eN
gcc eN.c -o eN
```

| Ex | Comando | Saída esperada |
|----|---------|----------------|
| e1 | `./e1` | `Tamanho: 634 bytes` |
| e2 | `./e2` e digite `lorem.txt` | 100 bytes do início + 50 bytes a partir do byte 200 |
| e3 | `./e3` e digite `lorem.txt` | tamanho, links, UID, `rw-rw-r--`, data |
| e4 | `./e4` | `stat` = 634 bytes (alvo), `lstat` = 9 bytes (link) + explicação |
| e5 | `./e5` ou `./e5 .` | mesmo tamanho/tipo pelos dois caminhos + `iguais? SIM` |
| e6 | `./e6` ou `./e6 <arquivo>` | `Numero de caracteres nulos:13485` |
| e7 | `./e7` | `01234567890123456789FIM`, arquivo fica com **24 bytes** |
| e8 | `./e8 t1.txt link .` | tamanho, tipo e permissões de cada um |

Diferente dos originais, o **e5 e o e6 agora aceitam o arquivo como argumento**
(com o mesmo padrão de antes se você não passar nada), o que permite testar o e5
com diretório: `./e5 .`

⚠️ **e7 destrói o `lorem.txt`.** Para rodar de novo:

```bash
cd e7
echo "01234567890123456789A_PARTIR_DAQUI_TUDO_VAI_SER_APAGADO_E_CORTADO" > lorem.txt
./e7
```

## Conferindo que as correções funcionam

```bash
# e3: permissoes de verdade (antes dizia "rwx" pra tudo)
cd e3 && echo "x" > t.txt && chmod 400 t.txt && ls -l t.txt && echo "t.txt" | ./e3

# e7: 24 bytes limpos (antes 25, com um \0 invisivel no fim)
cd e7 && stat -c%s lorem.txt && od -c lorem.txt

# e8: bate com o ls -l
cd e8 && ./e8 t1.txt link . && ls -ld t1.txt link .
```
