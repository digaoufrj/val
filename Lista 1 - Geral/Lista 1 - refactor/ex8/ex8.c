#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

void mostra_tipo(mode_t modo)
{
    if (S_ISREG(modo))
        printf("Arquivo Regular");
    else if (S_ISDIR(modo))
        printf("Diretorio");
    else if (S_ISLNK(modo))
        printf("Link Simbolico");
    else
        printf("Outro");
}

void mostra_permissoes(mode_t modo)
{
    // primeiro caractere eh o tipo, igual no ls -l
    printf((S_ISDIR(modo)) ? "d" : (S_ISLNK(modo) ? "l" : "-"));

    printf((modo & S_IRUSR) ? "r" : "-");
    printf((modo & S_IWUSR) ? "w" : "-");
    printf((modo & S_IXUSR) ? "x" : "-");
    printf((modo & S_IRGRP) ? "r" : "-");
    printf((modo & S_IWGRP) ? "w" : "-");
    printf((modo & S_IXGRP) ? "x" : "-");
    printf((modo & S_IROTH) ? "r" : "-");
    printf((modo & S_IWOTH) ? "w" : "-");
    printf((modo & S_IXOTH) ? "x" : "-");
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso correto: %s <arquivo1> <arquivo2> ...\n", argv[0]);
        return 1;
    }

    struct stat info;

    // comeca no 1 porque o argv[0] eh o nome do proprio programa
    for (int i = 1; i < argc; i++)
    {
        printf("\n--- Arquivo: %s ---\n", argv[i]);

        // lstat e nao stat, senao o link aparece como se fosse o arquivo alvo
        if (lstat(argv[i], &info) == -1)
        {
            perror("Erro ao ler o arquivo");
            continue;
        }

        printf("Tamanho: %ld bytes\n", (long)info.st_size);

        printf("Tipo: ");
        mostra_tipo(info.st_mode);
        printf("\n");

        printf("Permissoes: ");
        mostra_permissoes(info.st_mode);
        printf("\n");
    }

    return 0;
}
