#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso correto: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // um espaco a mais em cada um pro '\0', porque o read nao coloca terminador
    char buf[101] = {0};
    char buf2[51] = {0};

    ssize_t lidos = read(fd, buf, 100);
    if (lidos > 0)
    {
        printf("--- Primeiros 100 bytes ---\n%s\n", buf);
    }
    else
    {
        perror("Erro ao ler os primeiros 100 bytes ou arquivo vazio");
    }

    // SEEK_SET conta do inicio do arquivo, entao cai direto no byte 200
    off_t pos = lseek(fd, 200, SEEK_SET);
    if (pos == -1)
    {
        perror("Erro no lseek");
        close(fd);
        return 1;
    }

    ssize_t lidos2 = read(fd, buf2, 50);
    if (lidos2 > 0)
    {
        printf("\n--- 50 bytes a partir do byte 200 ---\n%s\n", buf2);
    }
    else
    {
        printf("\n--- O arquivo não é grande o suficiente para ler o byte 200 ---\n");
    }

    close(fd);
    return 0;
}
