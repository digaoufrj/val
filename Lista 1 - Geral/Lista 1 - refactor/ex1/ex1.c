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

    // o lseek devolve a posicao onde o cursor parou, entao mandando ele
    // pro fim essa posicao ja eh o tamanho do arquivo
    off_t tam = lseek(fd, 0, SEEK_END);
    if (tam == -1)
    {
        perror("Erro no lseek");
        close(fd);
        return 1;
    }

    printf("O tamanho do arquivo '%s' é: %ld bytes.\n", argv[1], (long)tam);

    close(fd);
    return 0;
}
