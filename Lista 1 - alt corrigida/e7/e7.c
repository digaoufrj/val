#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
int main()
{
	char frase[] = "FIM\n";
	int file = open("lorem.txt",O_RDWR);
	if(file == -1) // CORRIGIDO: faltava checar
	{
		perror("Erro ao abrir lorem.txt");
		return 1;
	}

	// CORRIGIDO: era SEEK_CUR. Dava no mesmo por o arquivo ter acabado de abrir
	// (cursor em 0), mas SEEK_SET diz exatamente o que o enunciado pede: byte 20
	lseek(file,20,SEEK_SET);

	// CORRIGIDO: era sizeof(frase), que vale 5 e nao 4, porque conta o '\0' do fim
	// da string. Isso gravava um byte nulo invisivel dentro do arquivo e o
	// resultado ficava com 25 bytes em vez de 24.
	write(file,frase,strlen(frase));
	
	int pos_atual = lseek(file,0,SEEK_CUR);

	ftruncate(file,pos_atual);

	lseek(file,0,SEEK_SET);

	char buffer[30];
	int lidos = read(file,buffer,pos_atual);
	buffer[lidos] = '\0'; // CORRIGIDO: read() nao poe terminador, o %s lia lixo depois
	printf("Texto truncado:%s",buffer);
	close(file); // CORRIGIDO: faltava fechar
	return 0;
}
