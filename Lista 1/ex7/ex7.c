#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // 1. Verifica se passou o nome do arquivo
    if (argc != 2)
    {
        printf("Uso correto: %s <nome_do_arquivo>\n", argv[0]);
        return 1;
    }

    // 2. Abre o arquivo para ESCRITA (O_WRONLY)
    int fd = open(argv[1], O_WRONLY);
    if (fd == -1)
    {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // 3. Estaciona o "carrinho" (cursor) exatamente no byte 20
    lseek(fd, 20, SEEK_SET);

    // 4. Escreve a palavra "FIM\n" (que tem exatos 4 bytes)
    write(fd, "FIM\n", 4);

    // 5. A "tesoura" do Linux: corta o arquivo na posição atual do carrinho.
    // (A professora pediu para truncar/cortar o que vier depois)
    off_t posicao_atual = lseek(fd, 0, SEEK_CUR);
    ftruncate(fd, posicao_atual);

    // Fecha o arquivo
    close(fd);
    return 0;
}