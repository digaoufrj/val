#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<errno.h>
int main()
{
	int file;
	struct stat info;

	file = open("lorem.txt",O_RDONLY  | O_NOFOLLOW);
	
	if(file == -1 && errno == ELOOP)
	{
		printf("É um link sombólico");
		return 0;
	}
	if(fstat(file,&info) == -1)
	{
		perror("Deu ruim no arquivo\n");
		return 1;
	}

   	 if (S_ISREG(info.st_mode))
    		printf("É um arquivo regular\n");
	else if (S_ISDIR(info.st_mode))
   		 printf("É um diretório\n");
	return 0;
}
