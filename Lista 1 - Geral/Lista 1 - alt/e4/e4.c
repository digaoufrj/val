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

	if(lstat(lkfile,&linfo) == -1)
	{
		perror("deu ruim no link");
		return 1;
	}
	
	printf("\nTamanho do link via lstat:%ld\n",(long)linfo.st_size);
	return 0;
}
