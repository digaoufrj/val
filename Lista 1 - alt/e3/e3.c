#include<sys/stat.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

int main()
{
	char name[20];
	struct stat file_info;

	printf("Digite o nome do arquivo:");
	fgets(name,sizeof(name),stdin);
	
	name[strcspn(name,"\n")] = '\0';

	if(stat(name,&file_info) == -1)
	{
		perror("Deu ruim aqui\n");	
		return 1;
	}

	printf("Tamanho:%ld\nNumero de links:%ld\nUID:%d\n",(long)file_info.st_size,(long)file_info.st_nlink,file_info.st_uid);

	printf((file_info.st_mode,S_IRUSR)? "r":"-");
	printf((file_info.st_mode,S_IWUSR)? "w":"-");
	printf((file_info.st_mode,S_IXUSR)? "x":"-");

	char data[100];
   	struct tm *tm_info = localtime(&file_info.st_mtime);
   	strftime(data, sizeof(data), "%d/%m/%Y %H:%M:%S", tm_info);
   	printf("\nUltima modificacao: %s\n", data);

	return 0;

}
