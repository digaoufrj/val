#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() 
{

	int cursor;
	off_t tam_bytes;

	cursor = open("lorem.txt",O_RDONLY); //abre o arquivo lorem.txt apenas para leitura e coloca o cursor do mouse no inicio do arquivo, na posição 0, então cursor = 0

	if(cursor == -1)
	{
		perror("Erro ao abrir lorem.txt");
		return 1;
	}

	tam_bytes = lseek(cursor,0,SEEK_END); // caminha com o cursor do texto do inicio do arquivo(posição 0 ) até o final do arquivo(SEEK_END) e chegando lá não se movimenta.Retorna a posição atual do cursor

	if(tam_bytes == -1)
	{
		perror("Erro no lseek");
		close(cursor);
		return 1;
	}

	printf("Tamanho: %ld bytes\n",(long)tam_bytes);

	close(cursor);
	return 0;
}
