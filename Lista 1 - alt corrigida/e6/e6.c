#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
	const char *nome = (argc > 1) ? argv[1] : "binario.bin";

	int cursor = open(nome,O_RDONLY);
	if(cursor == -1)
	{
		perror("Erro ao abrir o arquivo");
		return 1;
	}

	off_t tamanho = lseek(cursor,0,SEEK_END);
	off_t i = 0;
	int count = 0;
	char letra;

	while(i < tamanho)
	{
		lseek(cursor,i,SEEK_SET);
		read(cursor,&letra,1);
		if(letra == '\0') count += 1;
		i++;
	}	
	printf("Numero de caracteres nulos:%d\n",count);
	close(cursor);
	return 0;
}
