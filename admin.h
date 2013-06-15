#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdbool.h>

int s_fifo_fd;   /* identificador do FIFO do servidor */
int c_fifo_fd;   /* identificador do FIFO deste cliente */
char c_fifo_fname[25]; /* nome do FIFO deste cliente */

void showHelp()
{
    CLEAR;
    printf("\t\t\tManual de Utilização\n\n");
    printf("Comandos Disponíveis:\n\n");
    printf("\tshutdown - Encerra servidor e desliga terminal\n");
    printf("\taddcity  - Adiciona cidade  addcity <nome_cidade>\n");
    printf("\tdelcity  - Elimina cidade delcity <nome_cidade>\n");
    printf("\tseepast  - Mostra historico de voos\n");
    printf("\taddvoo   - Adiciona novo voo addvoo <id> <origem> <destino> <dia>\n");
    printf("\tcancel   - Cancela o voo pelo id cancel <id>\n");
    printf("\tmudadata - Muda data do sistema mudadadata <dia>\n");
    printf("\tgetdata  - Mostra data actual\n");
    printf("\tinfo     - Mostra terminais activos\n");
    printf("\tadduser  - Adiciona novo agente adduser <username> <password>\n");
    printf("\texit     - Termina programa e voltao à shell\n\n");

}

void sig_int(int i)
{
    fprintf(stderr, "\n[ADMIN]: A Encerrar.. (interrompido via teclado)\n");
    sleep(1);
    close(c_fifo_fd);
    close(s_fifo_fd);
    unlink(c_fifo_fname);
    printf("\nPrograma Terminou com Exito!\n");
    exit(EXIT_SUCCESS);
}

void HandleShutdown(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("\n[SERVER]: Comando para Shutdown Recebido\n");
        printf("[ADMIN]: A Encerrar..\n");
        sleep(1);
        close(c_fifo_fd);
        close(s_fifo_fd);
        unlink(c_fifo_fname);
        printf("\nPrograma Terminou com Exito!\n");
        exit(EXIT_SUCCESS);
    }
}
