
#define CLEAR system("clear");

/* ficheiro correspondente ao login dos agentes */
#define AGENT_FILE "configs/SOAGENTES.txt"

/* ficheiro correspondente aos passageiros dos voos */
#define PASSENGERS_FILE "configs/passengers_flights.txt"

/* ficheiro corresponde ao voos que ja ocorreram */
#define OLD_FLIGHTS_FILE "configs/old_flights.txt"

/* ficheiro correspondente aos voos */
#define FLIGHTS_FILE "configs/flights.txt"

/* ficheiro correspondente as cidades */
#define CITYS_FILE "configs/cidades.txt"

/* ficheiro correspondente a data */
#define DATE_FILE "configs/SODATA.txt"

/* ficheiro correspondente a password do admin */
#define ADMIN_FILE "configs/SOADMPASS.txt"

/* ficheiro correspondente ao FIFO do servidor */
#define SERVER_FIFO "connection/server_fifo"

/* ficheiro correspondente ao FIFO do cliente n (n-> "%d") */
#define CLIENT_FIFO "connection/resp_%d_fifo"

/* tamanho máximo de pergunta */
#define TAM_MAX 200
#define MAX_SIZE 1024

/* estrutura da mensagem correspondente ao um pedido */
typedef struct
{
    pid_t pid_cliente;
    int status;
    char palavra[TAM_MAX];
    char secure[TAM_MAX];
    char password[TAM_MAX];
    char username[TAM_MAX];
    int client_type;
} pergunta_t;

/* estrutura da mensagem correspondente a uma resposta */
typedef struct
{
    char palavra[TAM_MAX];
    bool loggedIn;
    char code[5];
    /* Códigos
    1- shutdown do server
    2 - exit do client
    3 - para Read
    */
} resposta_t;

char * BufferSpaceAlloc(FILE **File, char *buffer)
{
    int FileLenght;

    fseek(*File, 0, SEEK_END); //coloca o ponteiro no fim do ficheiro
    FileLenght=ftell(*File); //obtem a comprimento do ficheiro
    fseek(*File, 0, SEEK_SET); //coloca o ponteiro no inicio do ficheiro

    buffer=(char *)malloc(FileLenght+1); //aloca o espaco

    if(!buffer) //verifica se alocou bem o espaço
    {
        printf("Erro ao alocar memoria!\n");
        exit(EXIT_FAILURE);
    }
    return buffer; //devolve variavel com espaço alocado
}

