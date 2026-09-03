#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<errno.h>

int main(int argc, char *argv[])
{
	struct stat info;
	int i =1;
	int file;

	while(i < argc) 
	{					
		if(lstat(argv[i],&info) == -1) return 1;

		printf("Tamanho texto %d:%ld\n",i,(long)info.st_size);
	
		if(S_ISREG(info.st_mode)) printf("É um arquivo regular\n");
		else if(S_ISLNK(info.st_mode)) printf("É um link simbolico\n");
		else if(S_ISDIR(info.st_mode)) printf("É um diretorio\n");
	
		printf((info.st_mode & S_IRUSR)? "r":"-");
		printf((info.st_mode & S_IWUSR)? "w":"-");
		printf((info.st_mode & S_IXUSR)? "x":"-");
		puts("\n");
		i++;
	}
	return 0;	
}
