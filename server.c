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
#include "client.h"
#include "city.h"
#include "agents.h"
#include "flight.h"
#include "server.h"

int main()
{
    pergunta_t perg;	/* mensagem do "tipo" pergunta */
    resposta_t resp;	/* mensagem do "tipo" resposta */

    int read_res ,i ,x, for_clients=0, action, shutdown, client_type, loggedIn, SODATA, r_seats;
    char c_fifo_fname[50];
    char *token; /* token para uso do strtok */
    char *username, *password, *currentUser, *id, *origem, *destino, *seats, *dia, *clientPID, *searchOrigem, *searchDestino;
    bool matchWord=false; /* para controlo de comando (existe 1/0) */

    SODATA = getData(DATE_FILE);
    UpdateFlights(SODATA);
    cliente=AddClient(cliente, 1, "ghost-client"); //adiciona um cliente ficticio
    signal(SIGINT, sig_int); //controla uso do CTRL+C

    /* Muda processo para background */
    switch(fork())
    {
    case 0:
    	break;
    default:
    	exit(0);
    }

    //Cria Fifo para recepao de comandos e abre-o
    if(mkfifo(SERVER_FIFO, 0777) == -1)
    {
        fprintf(stderr, "\nServidor ja esta a correr! Programa vai agora terminar.");
        exit(EXIT_FAILURE);
    }
    s_fifo_fd = open(SERVER_FIFO, O_RDONLY); /* bloqueante */
    if (s_fifo_fd == -1)
    {
        fprintf(stderr, "\nErro ao abrir o FIFO");
        exit(EXIT_FAILURE);
    }

    keep_alive_fd = open(SERVER_FIFO, O_WRONLY);


    while(1)
    {
        SODATA = getData(DATE_FILE);
        /* ---- a) OBTEM INFORMAÇÃO DO REQUEST ---- */
        read_res = read(s_fifo_fd, & perg, sizeof(perg));
        if (read_res < sizeof(perg))
        {
            if (!strncasecmp("fim",(char *) & perg,3)) break;
            else
            {
                fprintf(stderr, "\nPergunta incompleta!");
                continue;
            }
        }

        /* ---- b) INTERPRETA INFORMAÇÃO ---- */
        resp.loggedIn = false;
        client_type = perg.client_type; //obtem tipo de cliente (int)
        loggedIn = perg.status;
        /* 1º Fase - Gerir login do Cliente */

        if(loggedIn == 0) //cliente não está autenticado, trata-se disso
        {
            if(client_type == 1) //Administrador
            {
                printf("Handles Admin Loggin\n\n");
                if(strncmp(perg.password, getAdminPassword(ADMIN_FILE), 100) == 0)
                {
                    resp.loggedIn = true;
                    /* adiciona novo client à struct */
                    cliente=AddClient(cliente, perg.pid_cliente, perg.password);
                    currentUser = strdup(perg.username);
                    printf("-> Client '%d' added!\n", perg.pid_cliente);
                }
                else
                {
                    resp.loggedIn = false;
                    printf("-> Access Denied!\n");
                }
            }
            else if (client_type == 2) //Agente
            {
                printf("Handles Agent Loggin\n\n");
                if(LoginAgent(perg.username, perg.password) == 1)
                {
                    resp.loggedIn = true;
                    /* adiciona novo client à struct */
                    cliente=AddClient(cliente, perg.pid_cliente, perg.password);
                    currentUser = strdup(perg.username);
                    printf("-> Client '%d' added!\n", perg.pid_cliente);
                }
                else
                {
                    resp.loggedIn = false;
                    printf("-> Access Denied!\n");
                }
            }
            else if (client_type != 2 && client_type != 1) //Desconhecido
            {
                resp.loggedIn = false;
            }

            /* ---- OBTEM FILENAME PARA RESPOSTA ---- */

            sprintf(c_fifo_fname, CLIENT_FIFO, perg.pid_cliente);

            /* ---- c) ENVIA RESPOSTA ---- */

            c_fifo_fd = open(c_fifo_fname, O_WRONLY | O_NONBLOCK);
            if (c_fifo_fd != -1)
            {
                write(c_fifo_fd, & resp, sizeof(resp));
                close(c_fifo_fd); /* FECHA LOGO O PIPE ! */
            }
            else
                fprintf(stderr, "\nNinguem quis a resposta [%s]", resp.palavra);
        }
        else
        {
            //cliente está autenticado, tratar dos comandos
            /* 2ª Fase - Processar Comandos */
            resp.loggedIn = true;

            /* verificação se cliente é valido */
            if(!isValidUser(cliente, perg.pid_cliente, perg.password))
                abort();

            printf("Full Command -> '%s'\n", perg.palavra);
            token = strtok(perg.palavra, " ");

            if( client_type ==1 ) /* INTERPRETA COMANDOS DE ADMIN */
            {
                printf("Handles Admin Command\n\n");
                for(i=0; i<NPALAVRAS; i++)
                {
                    if(!strcasecmp(token, validAdminCommands[i]))
                    {
                        matchWord = true;
                        action = i;
                        break;
                    }
                }

                if(matchWord == true)
                {
                    FILE *oldFlightsFile = fopen(OLD_FLIGHTS_FILE, "r");
                    switch(action)
                    {

                    case 0: //shutdown

                        strcpy(resp.code,"1");
                        strcpy(resp.palavra, "Servidor desligado com sucesso.");
                        shutdown = 1;
                        DisconnectClients(cliente);
                        break;

                    case 1: //addcity <nome_cidade>

                        token = strtok(NULL, " ");
                        if(token == NULL)
                        {
                            strcpy(resp.palavra,"Argumento Invalido! Digite 'help' para ajuda");
                            break;
                        }
                        if(AddCity(token)==1)
                        {
                            strcpy(resp.palavra, "Adicionou com sucesso nova cidade!");
                            break;
                        }
                        else
                        {
                            strcpy(resp.palavra, "Erro! Nao foi possivel adicionar nova cidade!");
                            break;
                        }

                    case 2: //delcity <nome_cidade>

                        token = strtok(NULL, " ");
                        if(token == NULL)
                        {
                            strcpy(resp.palavra,"Argumento Invalido! Digite 'help' para ajuda");
                            break;
                        }
                        if(DeleteCity(token)==1)
                        {
                            strcpy(resp.palavra, "Cidade eliminada com sucesso!");
                            break;
                        }
                        else
                        {
                            strcpy(resp.palavra, "Erro! Nao foi possivel eliminar cidade!");
                            break;
                        }

                    case 3: //seepast

                        sprintf(c_fifo_fname, CLIENT_FIFO, perg.pid_cliente);
                        c_fifo_fd = open(c_fifo_fname, O_WRONLY | O_NONBLOCK);
                        id = BufferSpaceAlloc(&oldFlightsFile, id);
                        origem = BufferSpaceAlloc(&oldFlightsFile, origem);
                        destino = BufferSpaceAlloc(&oldFlightsFile, destino);
                        seats = BufferSpaceAlloc(&oldFlightsFile, seats);
                        dia = BufferSpaceAlloc(&oldFlightsFile, dia);
                        while (fscanf(oldFlightsFile, "%s %s %s %s %s", id, origem, destino, seats, dia) != EOF)
                        {
                            if(atoi(seats) == 5)
                            {
                                r_seats = 0;
                            }
                            else if(atoi(seats) == 4)
                            {
                                r_seats = 1;
                            }
                            else if(atoi(seats) == 3)
                            {
                                r_seats = 2;
                            }
                            else if(atoi(seats) == 2)
                            {
                                r_seats = 3;
                            }
                            else if(atoi(seats) == 1)
                            {
                                r_seats = 4;
                            }
                            else if(atoi(seats) == 0)
                            {
                                r_seats = 5;
                            }

                            sprintf(resp.palavra, "Voo %s de %s para %s com %d pessoas ocorreu no dia %s.", id, origem, destino, r_seats, dia);
                            write(c_fifo_fd, & resp, sizeof(resp));
                        }
                        fclose(oldFlightsFile);
                        free(id);
                        free(origem);
                        free(destino);
                        free(seats);
                        free(dia);
                        break;
                    case 4: //addvoo id origem destino dia
                        token=strtok(NULL, " ");
                        id = strdup(token);
                        if((token=strtok(NULL, " ")) == NULL) break;
                        origem = strdup(token);
                        if((token=strtok(NULL, " ")) == NULL) break;
                        destino = strdup(token);
                        if((token=strtok(NULL, " ")) == NULL) break;
                        dia = strdup(token);
                        SODATA = getData(DATE_FILE);
                        if( AddFlight(dia, destino, origem, id, SODATA) == 1 )
                        {
                            strcpy(resp.palavra, "Voo adicionado com sucesso!");
                            break;
                        }
                        else
                        {
                            strcpy(resp.palavra, "Nao foi possivel adicionar voo!");
                            break;
                        }
                        break;

                    case 5: //cancel id

                        token = strtok(NULL, " ");
                        if(token == NULL)
                        {
                            strcpy(resp.palavra,"ID Invalido! Digite 'help' para ajuda");
                            break;
                        }
                        if( RemoveFlight(token) == 1 )
                        {
                            strcpy(resp.palavra, "Voo removido com sucesso!");
                            break;
                        }
                        else
                        {
                            strcpy(resp.palavra, "Nao foi possivel remover voo!");
                            break;
                        }
                        break;

                    case 6: //mudadata dia

                        token = strtok(NULL, " ");
                        if(token == NULL)
                        {
                            strcpy(resp.palavra,"Data Invalida! Digite 'help' para ajuda");
                            break;
                        }
                        if( AlteraData(token, SODATA) == 1 )
                        {
                            SODATA = getData(DATE_FILE);
                            UpdateFlights(SODATA);
                            strcpy(resp.palavra, "Data alterada com sucesso!");
                            break;
                        }
                        else
                        {
                            strcpy(resp.palavra, "Nao foi possivel alterar a data!");
                            break;
                        }
                        break;

                    case 7: //getdata

                        SODATA = getData(DATE_FILE);
                        sprintf(resp.palavra, "Data Actual - %d", SODATA);
                        break;

                    case 8: //info

                        sprintf(resp.palavra, "Terminal -> %d", StorePID(cliente));
                        break;

                    case 9: //adduser username password

                        token = strtok(NULL, " ");
                        username = strdup(token);
                        printf("Username: %s\n", username);
                        if((token=strtok(NULL, " ")) == NULL) break;
                        password = strdup(token);
                        printf("Password: %s\n", password);
                        if(AddAgent(username, password) == 1)
                        {
                            strcpy(resp.palavra, "Agente adicionado com sucesso!");
                        }
                        else
                        {
                            strcpy(resp.palavra, "Agente ja existe!");
                        }
                        break;

                    case 10: //exit do cliente

                        strcpy(resp.code, "2");
                        RemoveClient(cliente, cliente, perg.pid_cliente);
                        break;

                    case 11: //help

                        strcpy(resp.palavra, "---");
                        break;

                    default:
                        break;
                    }
                }
                else
                {
                    /* Comando nao foi reconhecido */
                    strcpy(resp.palavra,"Comando não reconhecido! Digite 'help' para ajuda");
                }
                /* ---- OBTEM FILENAME DO FIFO PARA A RESPOSTA ---- */

                sprintf(c_fifo_fname, CLIENT_FIFO, perg.pid_cliente);

                /* ---- c) ENVIA RESPOSTA ---- */

                c_fifo_fd = open(c_fifo_fname, O_WRONLY | O_NONBLOCK);
                if (c_fifo_fd != -1)
                {
                    write(c_fifo_fd, & resp, sizeof(resp));
                    close(c_fifo_fd); /* FECHA LOGO O FIFO ! */
                    matchWord = false; //bloqueia o reconhecimento de comando novamente
                }
                else
                    fprintf(stderr, "\nNinguem quis a resposta [%s]", resp.palavra);
                if(shutdown == 1)
                    break;
            }
            else if( client_type == 2 ) /* INTERPRETA COMANDOS DE AGENTE */
            {
                printf("Handles Agent Command\n\n");
                for(x=0; x<NPALAVRAS_AGENT; x++)
                {
                    if(!strcasecmp(token, validAgentCommands[x]))
                    {
                        matchWord = true;
                        action = x;
                        break;
                    }
                }

                if(matchWord == true)
                {
                    FILE *FlightsFile = fopen(FLIGHTS_FILE, "r");
                    switch(action)
                    {

                    case 0: //mudapass passantiga passnova

                        if( EditaPassword(currentUser, strtok(NULL, " "), strtok(NULL, " ")) == 1 )
                        {
                            strcpy(resp.palavra, "Password alterada com sucesso!");
                            break;
                        }
                        else
                        {
                            strcpy(resp.palavra, "Password antiga errada!");
                            break;
                        }

                    case 1: //lista

                        sprintf(c_fifo_fname, CLIENT_FIFO, perg.pid_cliente);
                        c_fifo_fd = open(c_fifo_fname, O_WRONLY | O_NONBLOCK);
                        id = BufferSpaceAlloc(&FlightsFile, id);
                        origem = BufferSpaceAlloc(&FlightsFile, origem);
                        destino = BufferSpaceAlloc(&FlightsFile, destino);
                        seats = BufferSpaceAlloc(&FlightsFile, seats);
                        dia = BufferSpaceAlloc(&FlightsFile, dia);

                        while (fscanf(FlightsFile, "%s %s %s %s %s", id, origem, destino, seats, dia) != EOF)
                        {
                            sprintf(resp.palavra, "Voo com id %s de %s para %s no dia %s, contem %s lugares vagos.", id, origem, destino, dia, seats);
                            write(c_fifo_fd, & resp, sizeof(resp));
                        }
                        fclose(FlightsFile);
                        free(id);
                        free(origem);
                        free(destino);
                        free(seats);
                        free(dia);
                        break;


                    case 2: //pesquisa origem destino

                        //destino - origem
                        token = strtok(NULL, " ");
                        searchOrigem = strdup(token);
                        token = strtok(NULL, " ");
                        searchDestino = strdup(token);


                        if((SearchCity(searchDestino) == 0) || SearchCity(searchOrigem) == 0)
                        {
                            strcpy(resp.palavra, "Erro ao pesquisar voos!");
                            break;
                        }

                        sprintf(c_fifo_fname, CLIENT_FIFO, perg.pid_cliente);
                        c_fifo_fd = open(c_fifo_fname, O_WRONLY | O_NONBLOCK);
                        id = BufferSpaceAlloc(&FlightsFile, id);
                        origem = BufferSpaceAlloc(&FlightsFile, origem);
                        destino = BufferSpaceAlloc(&FlightsFile, destino);
                        seats = BufferSpaceAlloc(&FlightsFile, seats);
                        dia = BufferSpaceAlloc(&FlightsFile, dia);

                        while (fscanf(FlightsFile, "%s %s %s %s %s", id, origem, destino, seats, dia) != EOF)
                        {
                            if(strncmp(origem, searchOrigem, 100) == 0)
                            {
                                if(strncmp(destino, searchDestino, 100) == 0)
                                {
                                    sprintf(resp.palavra, "Voo com id %s de %s para %s no dia %s, contem %s lugares vagos.", id, origem, destino, dia, seats);
                                    write(c_fifo_fd, & resp, sizeof(resp));
                                }
                                else
                                    sprintf(resp.palavra, "Erro ao pesquisar voos!");
                            }
                        }
                        fclose(FlightsFile);
                        free(id);
                        free(origem);
                        free(destino);
                        free(seats);
                        free(dia);
                        break;

                    case 3: //marca id passaporte

                        if( MarcaFlight(strtok(NULL, " "), strtok(NULL, " ")) == 1 )
                        {
                            strcpy(resp.palavra, "Voo marcado com sucesso!");
                            break;
                        }
                        else
                        {
                            strcpy(resp.palavra, "Erro ao marcar voo!");
                            break;
                        }

                    case 4: //desmarca id passaporte

                        if( DesmarcaFlight(strtok(NULL, " "), strtok(NULL, " ")) == 1 )
                        {
                            strcpy(resp.palavra, "Voo desmarcado com sucesso!");
                            break;
                        }
                        else
                        {
                            strcpy(resp.palavra, "Erro ao desmarcar voo!");
                            break;
                        }

                    case 5: //exit do cliente

                        strcpy(resp.code, "2");
                        RemoveClient(cliente, cliente, perg.pid_cliente);
                        break;

                    case 6: //help

                        strcpy(resp.palavra, "---");
                        break;

                    default:
                        break;
                    }
                }
                else
                {
                    /* Comando nao foi reconhecido */
                    strcpy(resp.palavra,"Comando não reconhecido! Digite 'help' para ajuda");
                }
                /* ---- OBTEM FILENAME DO FIFO PARA A RESPOSTA ---- */

                sprintf(c_fifo_fname, CLIENT_FIFO, perg.pid_cliente);

                /* ---- c) ENVIA RESPOSTA ---- */

                c_fifo_fd = open(c_fifo_fname, O_WRONLY | O_NONBLOCK);
                if (c_fifo_fd != -1)
                {
                    write(c_fifo_fd, & resp, sizeof(resp));
                    close(c_fifo_fd); /* FECHA LOGO O FIFO ! */
                    matchWord = false; //bloqueia o reconhecimento de comando novamente
                }
                else
                    fprintf(stderr, "\nNinguem quis a resposta [%s]", resp.palavra);
                if(shutdown == 1)
                    break;
            }
        }
    }

    close(keep_alive_fd);
    close(s_fifo_fd);
    unlink(SERVER_FIFO);
    return 1;

}
