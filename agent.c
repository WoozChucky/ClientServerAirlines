#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "dict.h"
#include "agent.h"

int main()
{

    pergunta_t perg; /* mensagem do "tipo" pergunta */
    resposta_t resp; /* mensagem do "tipo" resposta */
    char buffer[MAX_SIZE]; /* para a leitura da palavra a traduzir */
    char keep_password[20], keep_username[20];
    long fflags;
    int i, read_res, server_p_id, compare_result=1, tentativas_login=0, client_type=2;
    CLEAR;

    signal(SIGUSR1, HandleShutdown);
    signal(SIGINT, sig_int); //controla uso do CTRL+C
    //Abrir fifo do servidor
    s_fifo_fd = open(SERVER_FIFO, O_WRONLY); /* bloqueante */
    if (s_fifo_fd == -1)
    {
        fprintf(stderr, "\nO servidor não está a correr\n");
        exit(EXIT_FAILURE);
    }

    //Cria fifo para receber resposta do servidor
    perg.pid_cliente = getpid();
    sprintf(c_fifo_fname, CLIENT_FIFO, perg.pid_cliente);
    if (mkfifo(c_fifo_fname, 0777) == -1)
    {
        fprintf(stderr, "\nErro no FIFO para a resposta (1)");
        close(s_fifo_fd);
        exit(EXIT_FAILURE);
    }

    //Abre o fifo para receber a resposta do servidor
    c_fifo_fd = open(c_fifo_fname, O_RDONLY | O_NONBLOCK);
    if (c_fifo_fd == -1)
    {
        fprintf(stderr, "\nErro no FIFO para a resposta (2)\n");
        close(s_fifo_fd);
        unlink(c_fifo_fname);
        exit(EXIT_FAILURE);
    }

    perg.palavra[TAM_MAX-1] = '\0';

    /* antes de mais, autenticar admin */
    do
    {
        if(tentativas_login > 0)
        {
            printf("[AGENT]: Erraste a password %d vezes!\n", tentativas_login);
            sleep(5);
        }
        if (tentativas_login > 2)
        {
            printf("[AGENT]: Antingiu o maximo de tentativas possivel! O programa terminou.\n ");
            close(c_fifo_fd);
            close(s_fifo_fd);
            unlink(c_fifo_fname);
            return;
        }

        CLEAR
        /* ---- b) OBTEM LOGIN ---- */
        printf("\t\tBem Vindo ao Vale das Flores AirLines\n\n");
        printf("\tIntroduz as credencias de acesso\n\n");

        printf("[AGENT] Introduza username: ");
        fgets(buffer, TAM_MAX-1, stdin);
        strtok(buffer, "\n");
        strncpy(perg.username,buffer,TAM_MAX-1); //copia o username para a struct a enviar
        strncpy(keep_username,buffer,TAM_MAX-1);

        printf("[AGENT] Introduz password: ");
        fgets(buffer, TAM_MAX-1, stdin);
        strtok(buffer, "\n");
        strncpy(perg.password,buffer,TAM_MAX-1); //copia a password para a struct a enviar
        strncpy(keep_password,buffer,TAM_MAX-1);

        /* COPIA DADOS A ENVIAR */
        perg.client_type = client_type; //envia tipo de cliente 1 - admin
        perg.status = 0; //flag para nao autenticado

        /* ---- c) ENVIA PASSWORD ---- */
        write(s_fifo_fd, & perg, sizeof(perg));

        sleep(1);

        read_res = read(c_fifo_fd, & resp, sizeof(resp));
        if (read_res == sizeof(resp))
        {
            if(resp.loggedIn == true)
            {
                compare_result = 0;
                continue;
            }
            else
            {
                printf("[AGENT]: Password errada! Tente novamente.\n");
                tentativas_login++;
            }
        }
        else
        {
            printf("Sem resposta ou resposta incompreensivel[%d]\n",read_res);
        }

    }
    while(compare_result != 0);

    /* A partir desde ponto o agente foi autenticado */

    printf("\n[AGENT]: Agente de viagens autenticado com sucesso!\n\n");
    sleep(1);

    showHelp();
    printf("\tIntroduza os comandos pretendidos\n\n");
    /* Ciclo Principal */

    while (1)   /* caso escreva "exit" o programa termina */
    {

        /* ---- a) OBTEM COMANDO ---- */
        printf("[AGENT] $ ");
        fgets(buffer, TAM_MAX-1, stdin);
        strtok(buffer, "\n");

        if (!strcasecmp("help",buffer))
        {
            showHelp();
        }

        if (!strcasecmp("logout",buffer))
        {
            printf("\n[AGENT]: Logout efectuado com sucesso!\n");
            break;
        }

        /* ---- b) TRATA DAS INFORMAÇÕES A ENVIAR ---- */
        strncpy(perg.palavra,buffer,TAM_MAX-1); //copia o comando lida do "buffer" para a "perg.palavra"
        strncpy(perg.secure,keep_password,TAM_MAX-1); //copia autenticação (password) para "perg.secure"
        strncpy(perg.username, keep_username, TAM_MAX-1);
        perg.client_type = client_type; //envia tipo de cliente 1 - admin
        perg.status = 1; //flag para autenticado

        /* ---- c) ENVIA  COMANDO ---- */
        write(s_fifo_fd, & perg, sizeof(perg));

        sleep(1); // espera 1 segundos para fazer sincronização e obter resposta

        c_fifo_fd = open(c_fifo_fname, O_RDONLY | O_NONBLOCK);
        if (c_fifo_fd == -1)
        {
            fprintf(stderr, "\nErro no FIFO para a resposta (2)\n");
        }

        /* ---- d) OBTEM A RESPOSTA ---- */
        while(read_res=read(c_fifo_fd, & resp, sizeof(resp)) > 0)
        {
            if(strncmp(resp.code, "1", 100)==0) //caso específico de shutdown
            {
                printf("\n[SERVER]: %s\n", resp.palavra);
                sleep(1);
                printf("[AGENT]: A encerrar..\n");
                break;
            }
            else if(strncmp(resp.code, "2", 100)==0) //caso específico de exit
            {
                printf("\n[AGENT]: A encerrar..\n");
                sleep(1);
                break;
            }
            else
            {
                printf("[SERVER]: %s \n", resp.palavra);
            }
        }
    }

    close(c_fifo_fd);
    close(s_fifo_fd);
    unlink(c_fifo_fname);

    printf("\nPrograma Terminou com Exito!\n");
    exit(EXIT_SUCCESS);
}
