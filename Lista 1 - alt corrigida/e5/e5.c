#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<errno.h>

// funcao auxiliar para nao repetir o mesmo if/else nas duas partes
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
	struct stat info;      // preenchido pelo fstat
	struct stat info2;     // preenchido pelo stat
	// CORRIGIDO: nome fixo em "lorem.txt". Agora aceita argumento e assim da para
	// testar tambem com diretorio (./e5 .), como o enunciado pede
	const char *nome = (argc > 1) ? argv[1] : "lorem.txt";

	file = open(nome,O_RDONLY  | O_NOFOLLOW);
	
	if(file == -1 && errno == ELOOP)
	{
		printf("E um link simbolico\n"); // corrigido o "sombolico"
		return 0;
	}
	if(file == -1) // CORRIGIDO: qualquer outro erro de open passava batido
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
	printf("Tamanho: %ld bytes\n",(long)info.st_size); // CORRIGIDO: nao mostrava o tamanho
	mostra_tipo(info.st_mode);

	close(file); // CORRIGIDO: faltava fechar

	// CORRIGIDO: essa parte inteira faltava. O enunciado pede "Compare com stat()
	// para verificar que o resultado e o mesmo", e o stat() nunca era chamado.
	if(stat(nome,&info2) == -1)
	{
		perror("Deu ruim no stat");
		return 1;
	}

	printf("\n--- stat() (recebe o nome \"%s\") ---\n",nome);
	printf("Tamanho: %ld bytes\n",(long)info2.st_size);
	mostra_tipo(info2.st_mode);

	// a comparacao explicita que o enunciado pede
	printf("\nOs dois resultados sao iguais? %s\n",
		(info.st_size == info2.st_size && info.st_mode == info2.st_mode) ? "SIM" : "NAO");

	return 0;
}
