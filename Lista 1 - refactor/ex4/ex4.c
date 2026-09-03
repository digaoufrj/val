#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    const char *nome_link = "link.txt";

    struct stat info_stat;
    struct stat info_lstat;

    printf("--- Testando stat() vs lstat() em um link simbolico ---\n\n");

    if (stat(nome_link, &info_stat) == -1)
    {
        perror("Erro no stat (o link ou o arquivo original existem?)");
        return 1;
    }
    printf("Usando stat(\"%s\"):\n", nome_link);
    printf("Tamanho retornado: %ld bytes\n", (long)info_stat.st_size);
    printf("(Esse é o tamanho do arquivo REAL alvo do link)\n\n");

    // o lstat para no link e nao segue ate o alvo
    if (lstat(nome_link, &info_lstat) == -1)
    {
        perror("Erro no lstat");
        return 1;
    }
    printf("Usando lstat(\"%s\"):\n", nome_link);
    printf("Tamanho retornado: %ld bytes\n", (long)info_lstat.st_size);
    printf("(Esse é o tamanho do PROPRIO LINK simbolico)\n");

    return 0;
}
