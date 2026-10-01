#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // 1. Verifica se passamos o nome do arquivo
    if (argc != 2)
    {
        printf("Uso correto: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    // 2. Abre o arquivo binário para leitura
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // 3. Descobre o tamanho total do arquivo (como fizemos no Ex1)
    off_t tamanho_total = lseek(fd, 0, SEEK_END);

    int contador_nulos = 0;
    char byte_atual;

    // 4. O loop que vai percorrer o arquivo inteiro, byte por byte
    for (off_t i = 0; i < tamanho_total; i++)
    {

        // Pula EXATAMENTE para a posição 'i'
        lseek(fd, i, SEEK_SET);

        // Lê apenas 1 único byte dessa posição
        read(fd, &byte_atual, 1);

        // Verifica se esse byte é um Nulo (\0)
        if (byte_atual == '\0')
        {
            contador_nulos++;
        }
    }

    // 5. Exibe o resultado
    printf("O arquivo '%s' possui %d bytes nulos (\\0).\n", argv[1], contador_nulos);

    close(fd);
    return 0;
}