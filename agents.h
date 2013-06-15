
int SearchAgent(char *username)
{
    FILE *agentsFile = fopen(AGENT_FILE, "r");
    char *tmp, *password;

    tmp = BufferSpaceAlloc(&agentsFile, username);
    password = BufferSpaceAlloc(&agentsFile, password);

    while (!feof(agentsFile))
    {
        fscanf(agentsFile, "%s", tmp);
        fscanf(agentsFile, "%s", password);
        if (strstr(tmp, username))
        {
            fclose(agentsFile);
            return 1;
        }
    }
    fclose(agentsFile);
    return 0;
}

int AddAgent(char *username, char *password)
{
    FILE *agentsFile = fopen(AGENT_FILE, "a");

    if(SearchAgent(username) == 1)
    {
        return 0;
    }

    fprintf(agentsFile, "%s %s\n", username, password);

    fclose(agentsFile);
    return 1;
}

void ShowAgentsInfo()
{
    FILE *agentsFile = fopen(AGENT_FILE, "r");
    char *username, *password, *lixo;

    username = BufferSpaceAlloc(&agentsFile, username);
    password = BufferSpaceAlloc(&agentsFile, password);
    lixo = BufferSpaceAlloc(&agentsFile, lixo);

    while (!feof(agentsFile))
    {
        fscanf(agentsFile, "%s", username);
        fscanf(agentsFile, "%s", password);
        printf("User: %s\nPass: %s\n\n", username, password);
    }

    fclose(agentsFile);
}

int LoginAgent(char *username, char *password)
{
    FILE *agentsFile = fopen(AGENT_FILE, "r");
    char *f_username, *f_password;

    f_username = BufferSpaceAlloc(&agentsFile, f_username);
    f_password = BufferSpaceAlloc(&agentsFile, f_password);

    while (!feof(agentsFile))
    {
        fscanf(agentsFile, "%s", f_username);
        fscanf(agentsFile, "%s", f_password);
        if(strstr(f_username, username))
        {
            if(strstr(f_password, password))
            {
                fclose(agentsFile);
                return 1;
            }
        }
    }
    fclose(agentsFile);
    return 0;
}

int VerifyPassword(char *username, char *password)
{

}

int EditaPassword(char *username, char *newpassword, char *oldpassword)
{
    char *f_username, *f_password;

    if( LoginAgent(username, oldpassword) != 1 )
    {
        return 0;
    }

    FILE *agentsFile = fopen(AGENT_FILE, "r");
    FILE *agentsFileTMP = fopen("configs/SOAGENTES_temp.txt", "a");

    f_username = BufferSpaceAlloc(&agentsFile, f_username);
    f_password = BufferSpaceAlloc(&agentsFile, f_password);

    while (fscanf(agentsFile, "%s %s", f_username, f_password) != EOF) //enquanto nao for fim do ficheiro
    {
        if(strncmp(f_username, username, 100) == 0)
        {
            fprintf(agentsFileTMP, "%s %s\n", username, newpassword);
        }
        else
        {
            fprintf(agentsFileTMP, "%s %s\n", f_username, f_password);
        }
    }
    fclose(agentsFile);
    fclose(agentsFileTMP);
    unlink(AGENT_FILE);
    rename("configs/SOAGENTES_temp.txt" , AGENT_FILE);
    return 1;
}

/*
int AddAgent(char *username, char *password)
{
	FILE *agentsFile = fopen(AGENT_FILE, "ab");
	char *sep=":", *comma=";";

	if(SearchAgent(username) == 1)
	{
		printf("Encontrou username ja existente!\n");
		return 0;
	}

	fwrite (username , sizeof(char), strlen(username), agentsFile);
	fwrite (sep, sizeof(char), strlen(sep), agentsFile);
	fwrite (password , sizeof(char), strlen(password), agentsFile);
	fwrite (comma, sizeof(char), strlen(comma), agentsFile);

	fclose(agentsFile);
	return 1;
}

void ShowAgentsInfo()
{
	FILE *agentsFile = fopen(AGENT_FILE, "rb");
	char *buffer, *sep=":", *comma=";";
	long lSize;

	 // obtain file size:
    fseek (agentsFile , 0 , SEEK_END);
    lSize = ftell (agentsFile);
    rewind (agentsFile);

    buffer = (char*) malloc (sizeof(char)*lSize);

	if (buffer == NULL) {
		    fprintf(stderr, "buffer allocation failed\n");
		    return;
	}

	while(fread(buffer, sizeof(char), 1, agentsFile))
		{
			printf("%s", buffer);
		}
	fclose(agentsFile);
}
*/
