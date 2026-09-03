#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // Verifica se o nome do arquivo foi passado
    if (argc != 2)
    {
        printf("Uso correto: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    // Abre o arquivo apenas para leitura
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // Criamos buffers com 1 espaço a mais para o terminador de string ('\0')
    char buffer1[101] = {0};
    char buffer2[51] = {0};

    // 1. Lê os primeiros 100 bytes
    ssize_t lidos1 = read(fd, buffer1, 100);
    if (lidos1 > 0)
    {
        printf("--- Primeiros 100 bytes ---\n%s\n", buffer1);
    }
    else
    {
        perror("Erro ao ler os primeiros 100 bytes ou arquivo vazio");
    }

    // 2. Pula direto para o byte 200 a partir do INÍCIO do arquivo
    off_t posicao = lseek(fd, 200, SEEK_SET);
    if (posicao == -1)
    {
        perror("Erro no lseek");
        close(fd);
        return 1;
    }

    // 3. Lê 50 bytes a partir da nova posição
    ssize_t lidos2 = read(fd, buffer2, 50);
    if (lidos2 > 0)
    {
        printf("\n--- 50 bytes a partir do byte 200 ---\n%s\n", buffer2);
    }
    else
    {
        printf("\n--- O arquivo não é grande o suficiente para ler o byte 200 ---\n");
    }

    // Fecha o arquivo
    close(fd);
    return 0;
}