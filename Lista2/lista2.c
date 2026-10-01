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
    int n;
    char linha[256];
    arquivo_log = fopen("zumbie.txt", "a+");

    if ((n = (argc == 1) ? 1 : atol(argv[1])) <= 0)
    {
        fprintf(stderr, "Use: %s [<n>]\n", argv[0]);
        return 1;
    }

    if (fork())
        exit(0);
    (void)signal(SIGTERM, trata_sinal);
    (void)signal(SIGINT, SIG_IGN);
    (void)signal(SIGQUIT, SIG_IGN);
    (void)signal(SIGHUP, SIG_IGN);

    while (1)
    {
        sleep(n);
        FILE *comando = popen("ps -eo pid,ppid,comm,stat | grep ' Z'", "r");
        if (comando != NULL)
        {
            while (fgets(linha, sizeof(linha), comando) != NULL)
            {
                fprintf(arquivo_log, "%s", linha);
                fflush(arquivo_log);
            }
        }
        pclose(comando);
    }
}
