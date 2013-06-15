
/* estrutura de dados correspondente a um cliente */
typedef struct client client, *pno;
struct client
{
    pid_t pid;
    char password[TAM_MAX];
    pno proximo;
};

int verify_struct(pno cliente)
{
    if (cliente == NULL) //verifica se a lista está vazia
        return 1;
    else
        return 0;
}

pno AddClient(pno cliente, pid_t pid, char password[TAM_MAX])
{
    pno novo, aux;
    novo = malloc(sizeof(client)); //aloca espaço

    if(novo == NULL) //verifica se alocou o espaço com sucesso
    {
        printf("Erro na alocacao de memoria!\n");
        return cliente;
    }
    //preenche os campos do nó a ser inserido
    novo->pid = pid;
    novo->proximo = NULL;
    strncpy(novo->password, password, TAM_MAX-1);
    if(verify_struct(cliente))
    {
        novo->proximo = cliente;
        cliente = novo;
    }
    else
    {
        //senao insere no fim da lista
        aux = cliente;
        while(aux->proximo != NULL)
            aux = aux->proximo;
        aux->proximo = novo;
    }
    return cliente;
}

bool isValidUser(pno cliente,  pid_t pid, char password[TAM_MAX])
{
    while(cliente != NULL)
    {
        //printf("'%d'<->''");
        if(cliente->pid == pid && strncmp(password, cliente->password, 100) == 0)
        {
            return true;
        }
        cliente = cliente -> proximo;
    }
    return false;
}

pno RemoveClient(pno cliente, pno cliente_auxiliar, pid_t pid)
{
    pno actual, anterior = NULL;
    actual = cliente_auxiliar;

    printf("Vai Eliminar: -> '%d'\n", pid);

    while(actual != NULL && actual->pid != pid) //percorre toda a lista ate encontrar o ProdID passado por parametro
    {
        anterior = actual;
        actual = actual->proximo;
    }
    if(actual==NULL)
    {
        return;
    }
    if(anterior==NULL)
    {
        cliente = actual->proximo;
    }
    else
    {
        anterior->proximo = actual->proximo;
    }
    free(actual);
    return;
}

void ShowConnectedClients(pno cliente)
{
    printf("\tShowing Active Clients\n\n");
    while(cliente != NULL)
    {
        printf("ID: %d\n", cliente->pid);
        printf("Password: %s\n\n", cliente->password);
        cliente = cliente -> proximo;
    }
}

void DisconnectClients(pno cliente)
{
    cliente = cliente -> proximo; //passa pelo ghost client
    while(cliente != NULL)
    {
        kill(cliente->pid, SIGUSR1);
        cliente = cliente -> proximo;
    }

}

int CountClients(pno cliente)
{
    int count;
    cliente = cliente -> proximo; //passa pelo ghost client

    while(cliente != NULL)
    {
        count++;
        cliente = cliente -> proximo;
    }
    return count;
}

int StorePID(pno cliente)
{
    int pid;
    cliente = cliente -> proximo; //passa pelo ghost client

    pid = cliente->pid;

    return pid;

}



