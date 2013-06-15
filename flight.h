
int SearchFlight(char *id)
{
    FILE *flightsFile = fopen(FLIGHTS_FILE, "r");
    char *tmp, *lixo;

    tmp = BufferSpaceAlloc(&flightsFile, tmp);
    lixo = BufferSpaceAlloc(&flightsFile, lixo);

    while (!feof(flightsFile))
    {
        fscanf(flightsFile, "%s %s %s %s %s", tmp, lixo, lixo, lixo, lixo);
        if (strstr(tmp, id))
        {
            fclose(flightsFile);
            return 1;
        }
    }
    fclose(flightsFile);
    return 0;
}

int AddFlight(char *dia, char *destino, char *origem, char *id, int SODATA)
{

    FILE *flightsFile = fopen(FLIGHTS_FILE, "a");

    printf("ID: %s\n", id);
    printf("Origem: %s\n", origem);
    printf("Destino: %s\n", destino);
    printf("Dia: %s\n\n", dia);

    if(SODATA > atoi(dia))
    {
        fclose(flightsFile);
        return 0;
    }

    if(SearchFlight(id) == 1)
    {
        fclose(flightsFile);
        return 0;
    }

    if(SearchCity(origem) == 0)
    {
        fclose(flightsFile);
        return 0;
    }

    if(SearchCity(destino) == 0)
    {
        fclose(flightsFile);
        return 0;
    }

    fprintf(flightsFile, "%s %s %s %d %s\n", id, origem, destino, 5, dia);

    fclose(flightsFile);
    return 1;
}

int RemoveFlight(char *id)
{
    char *f_id, *origem, *destino, *seats, *dia;
    if(SearchFlight(id) != 1)
    {
        return 0;
    }

    FILE *flightsFile = fopen(FLIGHTS_FILE, "r");
    FILE *flightsFileTMP = fopen("configs/flights_temp.txt", "a");

    f_id = BufferSpaceAlloc(&flightsFile, f_id);
    origem = BufferSpaceAlloc(&flightsFile, origem);
    destino = BufferSpaceAlloc(&flightsFile, destino);
    seats = BufferSpaceAlloc(&flightsFile, seats);
    dia = BufferSpaceAlloc(&flightsFile, dia);

    while (fscanf(flightsFile, "%s %s %s %s %s", f_id, origem, destino, seats, dia) != EOF) //enquanto nao for fim do ficheiro
    {
        if(strncmp(f_id, id, 100) != 0)
        {
            fprintf(flightsFileTMP, "%s %s %s %s %s\n", f_id, origem, destino, seats, dia);
        }
    }

    fclose(flightsFile);
    fclose(flightsFileTMP);
    unlink(FLIGHTS_FILE);
    rename("configs/flights_temp.txt" , FLIGHTS_FILE);
    return 1;
}

void UpdateFlights(int SODATA)
{
    FILE *flightsFile = fopen(FLIGHTS_FILE, "r");
    FILE *flightsFileTMP = fopen("configs/flights_temp.txt", "a");
    FILE *oldflightsFile = fopen(OLD_FLIGHTS_FILE ,"a");
    FILE *passengersFile = fopen(PASSENGERS_FILE, "r");
    FILE *passengersFileTMP = fopen("configs/passengers_temp.txt", "a");
    char *f_id, *origem, *destino, *seats, *dia, *p_id, *p_passaporte;

    f_id = BufferSpaceAlloc(&flightsFile, f_id);
    origem = BufferSpaceAlloc(&flightsFile, origem);
    destino = BufferSpaceAlloc(&flightsFile, destino);
    seats = BufferSpaceAlloc(&flightsFile, seats);
    dia = BufferSpaceAlloc(&flightsFile, dia);
    p_id = BufferSpaceAlloc(&passengersFile, p_id);
    p_passaporte = BufferSpaceAlloc(&passengersFile, p_passaporte);

    while (fscanf(flightsFile, "%s %s %s %s %s", f_id, origem, destino, seats, dia) != EOF)
    {
        if(atoi(dia) >= SODATA)
        {
            fprintf(flightsFileTMP, "%s %s %s %s %s\n", f_id, origem, destino, seats, dia);
        }
        else
        {
            fprintf(oldflightsFile, "%s %s %s %s %s\n", f_id, origem, destino, seats, dia);
            while(fscanf(passengersFile, "%s %s", p_id, p_passaporte) != EOF)
            {
                if(strncmp(f_id, p_id, 100) == 0)
                {
                }
                else
                {
                    fprintf(passengersFileTMP, "%s %s\n", p_id, p_passaporte);
                }
            }
        }
    }

    fclose(passengersFile);
    fclose(passengersFileTMP);
    unlink(PASSENGERS_FILE);
    rename("configs/passengers_temp.txt" , PASSENGERS_FILE);
    fclose(oldflightsFile);
    fclose(flightsFile);
    fclose(flightsFileTMP);
    unlink(FLIGHTS_FILE);
    rename("configs/flights_temp.txt" , FLIGHTS_FILE);
}

int FileSize(char *filename)
{
    int size;
    FILE *file = fopen(filename, "r");
    size = ftell(file);
    fclose(file);
    return size;
}

int CheckPassenger(char *id, char *passaporte)
{
    FILE *passengersFile = fopen(PASSENGERS_FILE, "r");
    char *f_id, *f_pass;
    f_id = BufferSpaceAlloc(&passengersFile, f_id);
    f_pass = BufferSpaceAlloc(&passengersFile, f_pass);

    while (fscanf(passengersFile, "%s %s", f_id, f_pass) != EOF)
    {
        if(strncmp(f_pass, passaporte, 100) == 0)
        {
            return 1;
        }
    }
    fclose(passengersFile);
    return 0;
}

int MarcaFlight(char *passaporte, char *id)
{
    if(SearchFlight(id) != 1)
    {
        return 0;
    }
    if(FileSize(PASSENGERS_FILE) > 0)
    {
        if(CheckPassenger(id, passaporte) == 1)
        {
            return 0;
        }
    }


    FILE *flightsFile = fopen(FLIGHTS_FILE, "r");
    FILE *flightsFileTMP = fopen("configs/flights_temp.txt", "a");
    FILE *passengersFile = fopen(PASSENGERS_FILE, "a");
    char *f_id, *origem, *destino, *seats, *dia;
    int n_seats=0;

    f_id = BufferSpaceAlloc(&flightsFile, f_id);
    origem = BufferSpaceAlloc(&flightsFile, origem);
    destino = BufferSpaceAlloc(&flightsFile, destino);
    seats = BufferSpaceAlloc(&flightsFile, seats);
    dia = BufferSpaceAlloc(&flightsFile, dia);

    while (fscanf(flightsFile, "%s %s %s %s %s", f_id, origem, destino, seats, dia) != EOF)
    {
        if(strncmp(f_id, id, 100) == 0)
        {
            n_seats = atoi(seats)-1; //verificar se é menor que zero
            if(n_seats < 0)
            {
                fclose(passengersFile);
                fclose(flightsFile);
                fclose(flightsFileTMP);
                unlink("configs/flights_temp.txt");
                return 0;
            }
            fprintf(flightsFileTMP, "%s %s %s %d %s\n", f_id, origem, destino, n_seats, dia);
            fprintf(passengersFile, "%s %s\n", f_id, passaporte);
        }
        else
        {
            fprintf(flightsFileTMP, "%s %s %s %s %s\n", f_id, origem, destino, seats, dia);
        }
    }

    fclose(passengersFile);
    fclose(flightsFile);
    fclose(flightsFileTMP);
    unlink(FLIGHTS_FILE);
    rename("configs/flights_temp.txt" , FLIGHTS_FILE);
    return 1;
}

int CheckBooking(char *id, char *passaporte)
{
    FILE *passengersFile = fopen(PASSENGERS_FILE, "r");
    char *f_id, *f_pass;
    f_id = BufferSpaceAlloc(&passengersFile, f_id);
    f_pass = BufferSpaceAlloc(&passengersFile, f_pass);

    while (fscanf(passengersFile, "%s %s", f_id, f_pass) != EOF)
    {
        if( (strncmp(f_pass, passaporte, 100) == 0) && (strncmp(id, f_id, 100) == 0) )
        {
            return 1;
        }
    }
    fclose(passengersFile);
    return 0;
}

int DesmarcaFlight(char *passaporte, char *id)
{
    if(SearchFlight(id) != 1)
    {
        return 0;
    }
    printf("Passou voo!\n");
    if(CheckBooking(id, passaporte) != 1)
    {
        return 0;
    }
    printf("Passou reserva!\n");

    FILE *flightsFile = fopen(FLIGHTS_FILE, "r");
    FILE *flightsFileTMP = fopen("configs/flights_temp.txt", "a");
    FILE *passengersFile = fopen(PASSENGERS_FILE, "r");
    FILE *passengersFileTMP = fopen("configs/passengers_temp.txt", "a");
    char *f_id, *origem, *destino, *seats, *dia, *p_id, *p_passaporte;
    int n_seats=0;

    f_id = BufferSpaceAlloc(&flightsFile, f_id);
    origem = BufferSpaceAlloc(&flightsFile, origem);
    destino = BufferSpaceAlloc(&flightsFile, destino);
    seats = BufferSpaceAlloc(&flightsFile, seats);
    dia = BufferSpaceAlloc(&flightsFile, dia);
    p_id = BufferSpaceAlloc(&passengersFile, p_id);
    p_passaporte = BufferSpaceAlloc(&passengersFile, p_passaporte);

    while (fscanf(flightsFile, "%s %s %s %s %s", f_id, origem, destino, seats, dia) != EOF)
    {
        if(strncmp(f_id, id, 100) == 0)
        {
            n_seats = atoi(seats)+1;
            printf("Seats: %d\n", n_seats);
            if(n_seats > 5)
            {
                fclose(passengersFile);
                fclose(flightsFile);
                fclose(flightsFileTMP);
                unlink("configs/flights_temp.txt");
                return 0;
            }
            fprintf(flightsFileTMP, "%s %s %s %d %s\n", f_id, origem, destino, n_seats, dia);
            //remove booking with id & passaporte
            while (fscanf(passengersFile, "%s %s", p_id, p_passaporte) != EOF)
            {
                if( (strncmp(p_passaporte, passaporte, 100) == 0) && (strncmp(id, p_id, 100) == 0) )
                {
                }
                else
                {
                    fprintf(passengersFileTMP, "%s %s\n", p_id, p_passaporte);
                }
            }

            fclose(passengersFile);
            fclose(passengersFileTMP);
            unlink(PASSENGERS_FILE);
            rename("configs/passengers_temp.txt" , PASSENGERS_FILE);
        }
        else
        {
            fprintf(flightsFileTMP, "%s %s %s %s %s\n", f_id, origem, destino, seats, dia);
        }
    }

    fclose(flightsFile);
    fclose(flightsFileTMP);
    unlink(FLIGHTS_FILE);
    rename("configs/flights_temp.txt" , FLIGHTS_FILE);
    return 1;
}

