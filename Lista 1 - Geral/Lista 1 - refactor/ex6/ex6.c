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

    // mesmo truque do ex1 pra saber ate onde ir
    off_t total = lseek(fd, 0, SEEK_END);

    int nulos = 0;
    char b;

    for (off_t i = 0; i < total; i++)
    {
        // ando com o lseek ate a posicao i e leio so 1 byte de la
        lseek(fd, i, SEEK_SET);

        read(fd, &b, 1);

        if (b == '\0')
        {
            nulos++;
        }
    }

    printf("O arquivo '%s' possui %d bytes nulos (\\0).\n", argv[1], nulos);

    close(fd);
    return 0;
}
