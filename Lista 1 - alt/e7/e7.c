#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
int main()
{
	char frase[] = "FIM\n";
	int file = open("lorem.txt",O_RDWR);

	lseek(file,20,SEEK_CUR);

	write(file,frase,sizeof(frase));
	
	int pos_atual = lseek(file,0,SEEK_CUR);

	ftruncate(file,pos_atual);

	lseek(file,0,SEEK_SET);

	char buffer[30];
	read(file,buffer,pos_atual);
	printf("Texto truncado:%s",buffer);
	return 0;
}
