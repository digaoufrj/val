#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main()
{
	int cursor = open("e5",O_RDONLY);
	int tamanho = lseek(cursor,0,SEEK_END);
	int i = 0;
	int count = 0;
	char letra;

	lseek(cursor,0,SEEK_SET);

	while(i < tamanho)
	{
		read(cursor,&letra,1);
		if(letra == '\0') count += 1;
		i++;
	}	
	printf("Numero de caracteres nulos:%d\n",count);
	return 0;
}
