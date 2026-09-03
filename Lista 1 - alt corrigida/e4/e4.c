#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>

int main()
{
	struct stat info;
	struct stat linfo; 
	char lkfile[10] = "link";
	
	if(stat(lkfile,&info) == -1) 
	{
		perror("deu ruim no arquivo\n");
		return 1;
	}

	printf("Tamanho do link via stat:%ld",(long)info.st_size);
	// CORRIGIDO: o enunciado pede "Explique a diferenca". So os dois numeros nao explicam nada.
	printf("  <- stat() segue o link e mostra o tamanho do ARQUIVO ALVO\n");

	if(lstat(lkfile,&linfo) == -1)
	{
		perror("deu ruim no link");
		return 1;
	}
	
	printf("Tamanho do link via lstat:%ld",(long)linfo.st_size);
	printf("  <- lstat() NAO segue o link e mostra o tamanho do PROPRIO LINK\n");

	printf("\nO link e um arquivo que guarda so o caminho do alvo como texto,\n");
	printf("por isso o tamanho dele e o numero de letras desse caminho.\n");

	return 0;
}
