# Lista 1 — versão alternativa (Robson Duarte)

Resolução da **mesma lista** feita por outro aluno, guardada aqui para comparação.

- **Autor:** Robson Duarte
- **Origem:** https://github.com/RobsDuarte/Learning-Linux
- **Cópia feita em:** 03/09/2026

O código está **exatamente como no repositório original** — nada foi corrigido,
renomeado ou reformatado. Os binários compilados dele também foram mantidos
(diferente da minha `Lista 1`, onde eles ficam no `.gitignore`), porque o
exercício 6 usa o binário `e6/e5` como arquivo de teste e sem ele não roda.

Ver [COMPARACAO.md](COMPARACAO.md) para a conferência exercício por exercício
contra a minha versão.

## Diferenças de uso em relação à minha versão

A maior parte dos programas dele **não recebe argumento na linha de comando**:
o nome do arquivo é fixo no código ou pedido via teclado. Então rode sempre de
dentro da pasta do exercício.

| Ex | Como recebe o arquivo | Comando |
|----|----------------------|---------|
| e1 | fixo: `lorem.txt` | `./e1` |
| e2 | digitado no teclado | `./e2` e digite `lorem.txt` |
| e3 | digitado no teclado | `./e3` e digite o nome |
| e4 | fixo: `link` | `./e4` |
| e5 | fixo: `lorem.txt` | `./e5` |
| e6 | fixo: `e5` (binário) | `./e6` |
| e7 | fixo: `lorem.txt` | `./e7` (destrutivo) |
| e8 | argumentos | `./e8 lorem.txt link .` |

Padrão para compilar e rodar:

```bash
cd eN
gcc eN.c -o eN
./eN
```

⚠️ **e2 e e3 leem o nome com `fgets` em buffer pequeno** (10 e 20 bytes). No e2
cabem só 9 caracteres — `lorem.txt` tem exatamente 9 e funciona; nome maior é
cortado e o `open` falha.

⚠️ **e7 destrói o `lorem.txt`.** Para rodar de novo, recrie antes:

```bash
cd e7
echo "01234567890123456789A_PARTIR_DAQUI_TUDO_SERA_CORTADO" > lorem.txt
./e7
```
