#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

// Função auxiliar para não repetir código na hora de verificar o tipo do arquivo
void imprimir_tipo_arquivo(mode_t modo)
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
    // Verifica se passamos o nome do arquivo no terminal
    if (argc != 2)
    {
        printf("Uso correto: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    struct stat buf_fstat;
    struct stat buf_stat;

    // --- PARTE 1: Usando fstat() ---
    // Para o fstat, PRECISAMOS abrir o arquivo primeiro para pegar o "crachá" (fd)
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Erro ao abrir o arquivo com open()");
        return 1;
    }

    if (fstat(fd, &buf_fstat) == -1)
    {
        perror("Erro no fstat");
        close(fd);
        return 1;
    }

    printf("--- Resultado do fstat() ---\n");
    printf("Tamanho: %ld bytes\n", (long)buf_fstat.st_size);
    printf("Tipo: ");
    imprimir_tipo_arquivo(buf_fstat.st_mode);

    // Já usamos o fstat, podemos fechar o arquivo
    close(fd);

    // --- PARTE 2: Usando stat() ---
    // O stat só precisa da string com o nome do arquivo (argv[1])
    if (stat(argv[1], &buf_stat) == -1)
    {
        perror("Erro no stat");
        return 1;
    }

    printf("\n--- Resultado do stat() ---\n");
    printf("Tamanho: %ld bytes\n", (long)buf_stat.st_size);
    printf("Tipo: ");
    imprimir_tipo_arquivo(buf_stat.st_mode);

    return 0;
}