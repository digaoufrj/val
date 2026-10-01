# val

Exercícios de programação em C para a faculdade (UFRJ).

## Lista 1 — arquivos

- **[Lista 1](Lista%201%20-%20Geral/Lista%201/README.md)** — minha resolução. Chamadas de sistema para
  manipulação de arquivos: `lseek`, `stat`, `fstat`, `lstat` (+ `open`, `read`,
  `write`, `ftruncate`).
- **[Lista 1 - refactor](Lista%201%20-%20Geral/Lista%201%20-%20refactor/README.md)** — a mesma
  resolução com os nomes de variável e os comentários revisados. Mesma estrutura
  e mesma saída, byte a byte.
- **[GUIA.md](Lista%201%20-%20Geral/Lista%201/GUIA.md)** — conceitos dos 4 instrumentos, explicação de
  cada trecho do código e como apresentar cada exercício.

### Comparação com a resolução de outro aluno

- **[Lista 1 - alt](Lista%201%20-%20Geral/Lista%201%20-%20alt/README.md)** — resolução da mesma lista
  feita por [Robson Duarte](https://github.com/RobsDuarte/Learning-Linux),
  **como está no repositório dele**, sem alterações.
  - **[COMPARACAO.md](Lista%201%20-%20Geral/Lista%201%20-%20alt/COMPARACAO.md)** — conferência
    exercício por exercício entre as duas versões.
- **[Lista 1 - alt corrigida](Lista%201%20-%20Geral/Lista%201%20-%20alt%20corrigida/README.md)** — a
  versão dele com os defeitos consertados.
  - **[CORRECOES.md](Lista%201%20-%20Geral/Lista%201%20-%20alt%20corrigida/CORRECOES.md)** — o que
    mudou em cada arquivo e por quê.

## Lista 2 — processos

- **[lista2_ajustado.c](Lista2/lista2_ajustado.c)** — daemon que acorda de n em n
  segundos e registra num log os processos ZOMBIE do sistema. Identifica os
  zumbis rodando o `ps` como filho e lendo a saída por um pipe (`popen`).
  Encerra com `SIGTERM`; aos demais sinais é invulnerável.
- **[lista2.c](Lista2/lista2.c)** — a primeira versão, antes dos ajustes.
- **[gerazumbi.c](Lista2/gerazumbi.c)** — programa do enunciado que fabrica os
  zumbis para testar o daemon.
- **[GUIA.md](Lista2/GUIA.md)** — conceitos desde o zero (processo, `fork`,
  zumbi, sinais, pipe), o que cada trecho do código faz e um roteiro de
  demonstração.

## Compilando

Os binários não vão pro repositório (exceto os originais do Robson, que vieram
junto do repo dele). Entre na pasta do exercício e rode `gcc exN.c -o exN`.
Instruções detalhadas, saída esperada e como recriar cada arquivo de teste estão
no README de cada pasta.
