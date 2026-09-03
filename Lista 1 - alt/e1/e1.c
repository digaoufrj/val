#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() 
{

	int cursor;
	signed int tam_bytes;

	cursor = open("lorem.txt",O_RDONLY); //abre o arquivo lorem.txt apenas para leitura e coloca o cursor do mouse no inicio do arquivo, na posição 0, então cursor = 0
	tam_bytes = lseek(cursor,0,SEEK_END); // caminha com o cursor do texto do inicio do arquivo(posição 0 ) até o final do arquivo(SEEK_END) e chegando lá não se movimenta.Retorna a posição atual do cursor

	printf("%d",tam_bytes);
	return 0;
}
