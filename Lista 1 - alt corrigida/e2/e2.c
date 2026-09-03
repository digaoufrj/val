#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{
	char name[256];  // CORRIGIDO: era [10], so cabia nome de 9 letras
	int cursor;
	char texto[200] = {0};   // CORRIGIDO: zerado, senao o printf %s imprime lixo
	char texto2[100] = {0};  // CORRIGIDO: idem
	int tam;

	printf("Digite o nome do arquivo: ");
	fgets(name,sizeof(name),stdin);

	name[strcspn(name,"\n")] = '\0'; // CORRIGIDO: fgets guarda o Enter no fim e o open falhava

	cursor = open(name,O_RDONLY);
	if(cursor == -1) // CORRIGIDO: faltava checar
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
	texto[tam] = '\0'; // CORRIGIDO: read() nao coloca terminador, %s precisa dele

	printf("Texto ->%s \nQuantidade lida:%d ",texto,tam);

	// CORRIGIDO: era lseek(cursor,100,SEEK_CUR). Dava no mesmo (100 lidos + 100 = 200),
	// mas so por sorte: se o read acima devolvesse menos de 100 bytes o salto erraria o alvo.
	// SEEK_SET vai direto para o byte 200, que e o que o enunciado pede.
	lseek(cursor,200,SEEK_SET);
	
	tam = read(cursor,texto2,50);
	if(tam == -1)
	{
		perror("Erro na leitura");
		close(cursor);
		return 1;
	}
	texto2[tam] = '\0'; // CORRIGIDO

	printf("\nTexto ->%s \nQuantidade lida:%d\n",texto2,tam);
	close(cursor);
	return 0;
}
