#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
int main()
{
	char frase[] = "FIM\n";
	int file = open("lorem.txt",O_RDWR);
	if(file == -1)
	{
		perror("Erro ao abrir lorem.txt");
		return 1;
	}

	lseek(file,20,SEEK_SET);

	write(file,frase,strlen(frase));
	
	int pos_atual = lseek(file,0,SEEK_CUR);

	ftruncate(file,pos_atual);

	lseek(file,0,SEEK_SET);

	char buffer[30];
	int lidos = read(file,buffer,pos_atual);
	buffer[lidos] = '\0';
	printf("Texto truncado:%s",buffer);
	close(file);
	return 0;
}
