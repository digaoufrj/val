#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{
	char name[256];
	int cursor;
	char texto[200] = {0};
	char texto2[100] = {0};
	int tam;

	printf("Digite o nome do arquivo: ");
	fgets(name,sizeof(name),stdin);

	name[strcspn(name,"\n")] = '\0';

	cursor = open(name,O_RDONLY);
	if(cursor == -1)
	{
		perror("Erro ao abrir o arquivo");
		return 1;
	}

	tam = read(cursor,texto,100);
	if(tam == -1)
	{
		perror("Erro na leitura");
		close(cursor);
		return 1;
	}
	texto[tam] = '\0';

	printf("Texto ->%s \nQuantidade lida:%d ",texto,tam);

	lseek(cursor,200,SEEK_SET);
	
	tam = read(cursor,texto2,50);
	if(tam == -1)
	{
		perror("Erro na leitura");
		close(cursor);
		return 1;
	}
	texto2[tam] = '\0';

	printf("\nTexto ->%s \nQuantidade lida:%d\n",texto2,tam);
	close(cursor);
	return 0;
}
