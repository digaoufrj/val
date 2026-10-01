#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

FILE *arquivo_log;

void trata_sinal(int sig)
{
    fprintf(arquivo_log, "Recebi um sinal para fechar\n");
    fclose(arquivo_log);
    exit(0);
}

int main(int argc, char **argv)
{
    int n, i;
    char linha[256];

    if ((n = (argc == 1) ? 1 : atol(argv[1])) <= 0)
    {
        fprintf(stderr, "Use: %s [<n>]\n", argv[0]);
        return 1;
    }

    arquivo_log = fopen("zumbie.txt", "a+");
    if (arquivo_log == NULL)
    {
        perror("zumbie.txt");
        return 1;
    }

    if (fork())
        exit(0);

    for (i = 1; i < NSIG; i++)
        if (i != SIGCHLD)
            (void)signal(i, SIG_IGN);
    (void)signal(SIGTERM, trata_sinal);

    fprintf(arquivo_log, "PID PPID Nome do Programa\n");
    fflush(arquivo_log);

    while (1)
    {
        sleep(n);
        FILE *comando = popen("ps -eo pid,ppid,comm,stat --no-headers | awk '$4 ~ /^Z/ {print $1, $2, $3}'", "r");
        if (comando != NULL)
        {
            fprintf(arquivo_log, "==========================================\n");
            while (fgets(linha, sizeof(linha), comando) != NULL)
            {
                fprintf(arquivo_log, "%s", linha);
            }
            fflush(arquivo_log);
            pclose(comando);
        }
    }
}
