#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// separei numa funcao pra nao repetir o mesmo if/else duas vezes
void mostra_tipo(mode_t modo)
{
    if (S_ISREG(modo))
    {
        printf("Arquivo Regular\n");
    }
    else if (S_ISDIR(modo))
    {
        printf("Diretorio\n");
    }
    else if (S_ISLNK(modo))
    {
        printf("Link Simbolico\n");
    }
    else
    {
        printf("Outro tipo (socket, device, etc)\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso correto: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    struct stat info_fd;
    struct stat info_nome;

    // o fstat so aceita descritor, entao tem que abrir o arquivo antes
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Erro ao abrir o arquivo com open()");
        return 1;
    }

    if (fstat(fd, &info_fd) == -1)
    {
        perror("Erro no fstat");
        close(fd);
        return 1;
    }

    printf("--- Resultado do fstat() ---\n");
    printf("Tamanho: %ld bytes\n", (long)info_fd.st_size);
    printf("Tipo: ");
    mostra_tipo(info_fd.st_mode);

    close(fd);

    // ja o stat trabalha direto com o nome
    if (stat(argv[1], &info_nome) == -1)
    {
        perror("Erro no stat");
        return 1;
    }

    printf("\n--- Resultado do stat() ---\n");
    printf("Tamanho: %ld bytes\n", (long)info_nome.st_size);
    printf("Tipo: ");
    mostra_tipo(info_nome.st_mode);

    return 0;
}
