#include <stdio.h>
#include <sys/stat.h> // Biblioteca essencial para a função stat()
#include <time.h>     // Biblioteca para formatar a data e hora

int main(int argc, char *argv[])
{
    // Verifica se passamos o nome do arquivo no terminal
    if (argc != 2)
    {
        printf("Uso correto: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    // Criamos uma "ficha" em branco (struct stat) para o Linux preencher
    struct stat info;

    // A função stat pega as informações do arquivo (argv[1]) e preenche a ficha (&info)
    if (stat(argv[1], &info) == -1)
    {
        perror("Erro ao obter informacoes com stat");
        return 1;
    }

    // Exibe os dados solicitados pelo exercício
    printf("--- INFORMAÇÕES DO ARQUIVO ---\n");
    printf("Tamanho: %ld bytes\n", (long)info.st_size);
    printf("Numero de links: %ld\n", (long)info.st_nlink);
    printf("UID do dono: %u\n", info.st_uid);

    // Permissões de acesso (rwx)
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

    // Formatação da última modificação (st_mtime) em data/hora
    char tempo_formatado[100];
    struct tm *tm_info = localtime(&info.st_mtime);
    strftime(tempo_formatado, sizeof(tempo_formatado), "%d/%m/%Y %H:%M:%S", tm_info);

    printf("Ultima modificacao: %s\n", tempo_formatado);

    return 0;
}