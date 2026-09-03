#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Uso correto: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    struct stat info;

    if (stat(argv[1], &info) == -1)
    {
        perror("Erro ao obter informacoes com stat");
        return 1;
    }

    printf("--- INFORMAÇÕES DO ARQUIVO ---\n");
    printf("Tamanho: %ld bytes\n", (long)info.st_size);
    printf("Numero de links: %ld\n", (long)info.st_nlink);
    printf("UID do dono: %u\n", info.st_uid);

    // as permissoes ficam bit a bit dentro do st_mode, entao testo cada
    // uma com & pra ver se aquele bit esta ligado
    printf("Permissoes de acesso: ");
    printf((S_ISDIR(info.st_mode)) ? "d" : "-");
    printf((info.st_mode & S_IRUSR) ? "r" : "-");
    printf((info.st_mode & S_IWUSR) ? "w" : "-");
    printf((info.st_mode & S_IXUSR) ? "x" : "-");
    printf((info.st_mode & S_IRGRP) ? "r" : "-");
    printf((info.st_mode & S_IWGRP) ? "w" : "-");
    printf((info.st_mode & S_IXGRP) ? "x" : "-");
    printf((info.st_mode & S_IROTH) ? "r" : "-");
    printf((info.st_mode & S_IWOTH) ? "w" : "-");
    printf((info.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n");

    // st_mtime vem em segundos desde 1970, o localtime quebra isso em
    // dia/mes/ano e o strftime monta a string
    char data[100];
    struct tm *tm_info = localtime(&info.st_mtime);
    strftime(data, sizeof(data), "%d/%m/%Y %H:%M:%S", tm_info);

    printf("Ultima modificacao: %s\n", data);

    return 0;
}
