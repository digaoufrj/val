#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<errno.h>

int main(int argc, char *argv[])
{
	struct stat info;
	int i =1;

	if(argc < 2)
	{
		printf("Uso: %s <arquivo1> <arquivo2> ...\n",argv[0]);
		return 1;
	}

	while(i < argc) 
	{					
		if(lstat(argv[i],&info) == -1)
		{
			perror(argv[i]);
			i++;
			continue;
		}

		printf("--- %s ---\n",argv[i]);
		printf("Tamanho: %ld bytes\n",(long)info.st_size);
	
		printf("Tipo: ");
		if(S_ISREG(info.st_mode)) printf("arquivo regular\n");
		else if(S_ISLNK(info.st_mode)) printf("link simbolico\n");
		else if(S_ISDIR(info.st_mode)) printf("diretorio\n");
		else printf("outro\n");
	
		printf("Permissoes: ");
		printf(S_ISDIR(info.st_mode) ? "d" : (S_ISLNK(info.st_mode) ? "l" : "-"));

		printf((info.st_mode & S_IRUSR)? "r":"-");
		printf((info.st_mode & S_IWUSR)? "w":"-");
		printf((info.st_mode & S_IXUSR)? "x":"-");
		printf((info.st_mode & S_IRGRP)? "r":"-");
		printf((info.st_mode & S_IWGRP)? "w":"-");
		printf((info.st_mode & S_IXGRP)? "x":"-");
		printf((info.st_mode & S_IROTH)? "r":"-");
		printf((info.st_mode & S_IWOTH)? "w":"-");
		printf((info.st_mode & S_IXOTH)? "x":"-");
		printf("\n\n");
		i++;
	}
	return 0;	
}
