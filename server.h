
#define NPALAVRAS 12
#define NPALAVRAS_AGENT 7

int s_fifo_fd, c_fifo_fd, keep_alive_fd; /* descrição do ficheiro */
pno cliente=NULL; /* client do "tipo" client_t */

char * validAdminCommands[NPALAVRAS] =
{
    "shutdown",
    "addcity",
    "delcity",
    "seepast",
    "addvoo",
    "cancel",
    "mudadata",
    "getdata",
    "info",
    "adduser",
    "exit",
    "help"
};

char * validAgentCommands[NPALAVRAS_AGENT] =
{
    "mudapass",
    "lista",
    "pesquisa",
    "marca",
    "desmarca",
    "exit",
    "help"
};

void sig_int(int i)
{
    DisconnectClients(cliente);
    fprintf(stderr, "\nServidor a terminar (interrompido via teclado)\n");
    close(keep_alive_fd);
    close(s_fifo_fd);
    unlink(SERVER_FIFO);
    exit(EXIT_SUCCESS);
    /* termina o processo */
}

char *getAdminPassword(char *filename)
{
    FILE *file;
    char *adminPassword = malloc (sizeof (char) * 50);
    file = fopen(filename, "r");
    if(file==NULL)
    {
        printf("[SERVER]: Nao foi possivel abrir ficheiro de administrador! Programa vai terminar\n");
        exit(EXIT_FAILURE);
    }
    fscanf(file, "%s", adminPassword);
    fclose(file);
    return adminPassword;
}

int getData(char *filename)
{
    FILE *file;
    int data;
    file = fopen(filename, "r");
    if(file==NULL)
    {
        exit(EXIT_FAILURE);
    }
    fscanf(file, "%d", &data);
    fclose(file);
    return data;
}

int AlteraData(char *novadata, int SODATA)
{

    if(SODATA > atoi(novadata))
    {
        return 0;
    }

    FILE *dataFile = fopen(DATE_FILE, "w");

    fprintf(dataFile, "%s", novadata);

    fclose(dataFile);
    return 1;
}

