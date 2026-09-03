#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<errno.h>

int main(int argc, char *argv[])
{
	struct stat info;
	int i =1;
	// CORRIGIDO: removida a variavel "file", que era declarada e nunca usada

	if(argc < 2) // CORRIGIDO: sem argumento o programa terminava sem dizer nada
	{
		printf("Uso: %s <arquivo1> <arquivo2> ...\n",argv[0]);
		return 1;
	}

	while(i < argc) 
	{					
		// CORRIGIDO: era "return 1", que abortava tudo no primeiro arquivo ruim.
		// Agora avisa e segue para os proximos
		if(lstat(argv[i],&info) == -1)
		{
			perror(argv[i]);
			i++;
			continue;
		}

		// CORRIGIDO: mostrava so o indice ("Tamanho texto 1"), agora mostra o nome
		printf("--- %s ---\n",argv[i]);
		printf("Tamanho: %ld bytes\n",(long)info.st_size);
	
		printf("Tipo: ");
		if(S_ISREG(info.st_mode)) printf("arquivo regular\n");
		else if(S_ISLNK(info.st_mode)) printf("link simbolico\n");
		else if(S_ISDIR(info.st_mode)) printf("diretorio\n");
		else printf("outro\n"); // CORRIGIDO: antes nao imprimia nada nesse caso
	
		// CORRIGIDO: faltava o caractere de tipo na frente (d/l/-), como no ls -l
		printf("Permissoes: ");
		printf(S_ISDIR(info.st_mode) ? "d" : (S_ISLNK(info.st_mode) ? "l" : "-"));

		printf((info.st_mode & S_IRUSR)? "r":"-");
		printf((info.st_mode & S_IWUSR)? "w":"-");
		printf((info.st_mode & S_IXUSR)? "x":"-");
		// CORRIGIDO: faltavam grupo e outros
		printf((info.st_mode & S_IRGRP)? "r":"-");
		printf((info.st_mode & S_IWGRP)? "w":"-");
		printf((info.st_mode & S_IXGRP)? "x":"-");
		printf((info.st_mode & S_IROTH)? "r":"-");
		printf((info.st_mode & S_IWOTH)? "w":"-");
		printf((info.st_mode & S_IXOTH)? "x":"-");
		printf("\n\n"); // CORRIGIDO: era puts("\n"), que ja adiciona um \n e saiam 2 linhas em branco
		i++;
	}
	return 0;	
}
