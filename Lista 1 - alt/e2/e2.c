#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{
	char name[10];
	int cursor;
	char texto[200];
	char texto2[100];
	int tam;

	printf("Digite o nome do arquivo: ");
	fgets(name,sizeof(name),stdin);

	cursor = open(name,O_RDONLY);

	tam = read(cursor,texto,100);

	printf("Texto ->%s \nQuantidade lida:%d ",texto,tam);

	lseek(cursor,100,SEEK_CUR);
	
	tam = read(cursor,texto2,50);
	printf("\nTexto ->%s \nQuantidade lida:%d",texto2,tam);
	close(cursor);
	return 0;
}
