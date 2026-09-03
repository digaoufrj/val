#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // Verifica se passamos o nome do arquivo na hora de rodar
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

    // Move o cursor para o final do arquivo e pega a posição
    off_t tamanho = lseek(fd, 0, SEEK_END);
    if (tamanho == -1)
    {
        perror("Erro no lseek");
        close(fd);
        return 1;
    }

    // Exibe o tamanho em bytes
    printf("O tamanho do arquivo '%s' é: %ld bytes.\n", argv[1], (long)tamanho);

    // Fecha o arquivo
    close(fd);
    return 0;
}
