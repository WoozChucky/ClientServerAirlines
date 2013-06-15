
int s_fifo_fd;   /* identificador do FIFO do servidor */
int c_fifo_fd;   /* identificador do FIFO deste cliente */
char c_fifo_fname[25]; /* nome do FIFO deste cliente */

void showHelp()
{
    CLEAR;
    printf("\t\t\tManual de Utilização\n\n");
    printf("Comandos Disponíveis:\n\n");
    printf("\tmudapass - Altera password. mudapass <antiga> <nova>\n");
    printf("\tlista    - Lista voos por realizar\n");
    printf("\tpesquisa - Pesquisas voos entre <origem> <destino>\n");
    printf("\tmarca    - Marca voo <id_voo> <num_passaporte>\n");
    printf("\tdesmarca - Desmarca voo desmarca <id_voo> <num_passaporte>\n");
    printf("\tlogout   - Sai da conta actual\n");
    printf("\texit     - Termina programa\n\n");

}

void sig_int(int i)
{
    fprintf(stderr, "\n[AGENT]: A Encerrar.. (interrompido via teclado)\n");
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
        printf("[AGENT]: A Encerrar..\n");
        sleep(1);
        close(c_fifo_fd);
        close(s_fifo_fd);
        unlink(c_fifo_fname);
        printf("\nPrograma Terminou com Exito!\n");
        exit(EXIT_SUCCESS);
    }
}
