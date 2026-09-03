#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso correto: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_WRONLY);
    if (fd == -1)
    {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    lseek(fd, 20, SEEK_SET);

    // 4 na mao e nao sizeof, porque sizeof("FIM\n") daria 5 e o '\0'
    // do fim da string acabaria gravado dentro do arquivo
    write(fd, "FIM\n", 4);

    // escrever no meio nao apaga o resto sozinho, quem corta eh o ftruncate
    off_t pos = lseek(fd, 0, SEEK_CUR);
    ftruncate(fd, pos);

    close(fd);
    return 0;
}
