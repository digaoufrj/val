# val

Exercícios de programação em C para a faculdade (UFRJ).

## Conteúdo

- **[Lista 1](Lista%201/README.md)** — minha resolução. Chamadas de sistema para
  manipulação de arquivos: `lseek`, `stat`, `fstat`, `lstat` (+ `open`, `read`,
  `write`, `ftruncate`).
- **[Lista 1 - refactor](Lista%201%20-%20refactor/README.md)** — a mesma
  resolução com os nomes de variável e os comentários revisados. Mesma estrutura
  e mesma saída, byte a byte.
- **[GUIA.md](Lista%201/GUIA.md)** — conceitos dos 4 instrumentos, explicação de
  cada trecho do código e como apresentar cada exercício.

### Comparação com a resolução de outro aluno

- **[Lista 1 - alt](Lista%201%20-%20alt/README.md)** — resolução da mesma lista
  feita por [Robson Duarte](https://github.com/RobsDuarte/Learning-Linux),
  **como está no repositório dele**, sem alterações.
  - **[COMPARACAO.md](Lista%201%20-%20alt/COMPARACAO.md)** — conferência
    exercício por exercício entre as duas versões.
- **[Lista 1 - alt corrigida](Lista%201%20-%20alt%20corrigida/README.md)** — a
  versão dele com os defeitos consertados.
  - **[CORRECOES.md](Lista%201%20-%20alt%20corrigida/CORRECOES.md)** — o que
    mudou em cada arquivo e por quê.

## Compilando

Os binários não vão pro repositório (exceto os originais do Robson, que vieram
junto do repo dele). Entre na pasta do exercício e rode `gcc exN.c -o exN`.
Instruções detalhadas, saída esperada e como recriar cada arquivo de teste estão
no README de cada pasta.
