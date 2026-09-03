#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h> // <-- A LINHA MÁGICA QUE FALTAVA

// Função para imprimir o tipo
void imprimir_tipo(mode_t modo)
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

// Função para imprimir as permissões rwx
void imprimir_permissoes(mode_t modo)
{
    // Primeiro caractere indica o tipo (d = diretório, l = link, - = normal)
    printf((S_ISDIR(modo)) ? "d" : (S_ISLNK(modo) ? "l" : "-"));

    // Peneira das permissões rwx
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
    // Verifica se o usuário passou pelo menos UM arquivo
    if (argc < 2)
    {
        printf("Uso correto: %s <arquivo1> <arquivo2> ...\n", argv[0]);
        return 1;
    }

    struct stat info;

    // Loop começando do 1 para ignorar o ./ex8
    for (int i = 1; i < argc; i++)
    {
        printf("\n--- Arquivo: %s ---\n", argv[i]);

        // lstat no lugar do stat para pegar atalhos também
        if (lstat(argv[i], &info) == -1)
        {
            perror("Erro ao ler o arquivo");
            continue;
        }

        printf("Tamanho: %ld bytes\n", (long)info.st_size);

        printf("Tipo: ");
        imprimir_tipo(info.st_mode);
        printf("\n");

        printf("Permissoes: ");
        imprimir_permissoes(info.st_mode);
        printf("\n");
    }

    return 0;
}