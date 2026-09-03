#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<errno.h>

void mostra_tipo(mode_t modo)
{
	if (S_ISREG(modo))       printf("E um arquivo regular\n");
	else if (S_ISDIR(modo))  printf("E um diretorio\n");
	else if (S_ISLNK(modo))  printf("E um link simbolico\n");
	else                     printf("E outro tipo\n");
}

int main(int argc, char *argv[])
{
	int file;
	struct stat info;
	struct stat info2;
	const char *nome = (argc > 1) ? argv[1] : "lorem.txt";

	file = open(nome,O_RDONLY  | O_NOFOLLOW);
	
	if(file == -1 && errno == ELOOP)
	{
		printf("E um link simbolico\n");
		return 0;
	}
	if(file == -1)
	{
		perror("Erro ao abrir o arquivo");
		return 1;
	}

	if(fstat(file,&info) == -1)
	{
		perror("Deu ruim no arquivo\n");
		close(file);
		return 1;
	}

	printf("--- fstat() (recebe o descritor %d) ---\n",file);
	printf("Tamanho: %ld bytes\n",(long)info.st_size);
	mostra_tipo(info.st_mode);

	close(file);

	if(stat(nome,&info2) == -1)
	{
		perror("Deu ruim no stat");
		return 1;
	}

	printf("\n--- stat() (recebe o nome \"%s\") ---\n",nome);
	printf("Tamanho: %ld bytes\n",(long)info2.st_size);
	mostra_tipo(info2.st_mode);

	printf("\nOs dois resultados sao iguais? %s\n",
		(info.st_size == info2.st_size && info.st_mode == info2.st_mode) ? "SIM" : "NAO");

	return 0;
}
