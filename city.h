
int GetNumberOfCitys()
{
    int n, ch;
    FILE *citysFile = fopen(CITYS_FILE, "r");

    if(citysFile == NULL)
    {
        return;
    }

    while ((ch=fgetc(citysFile)) != EOF)
        if (ch==10)
            n++;

    fclose(citysFile);
    return n;
}

int SearchCity(char *city_name)
{
    FILE *citysFile = fopen(CITYS_FILE, "r");
    char tmp[256];
    int line=1;

    while(citysFile!=NULL && fgets(tmp, sizeof(tmp),citysFile)!=NULL)
    {
        if (strstr(tmp, city_name))
        {
            fclose(citysFile);
            return line;
        }
        line++;
    }
    fclose(citysFile);
    return 0;
}

int AddCity(char *city_name)
{
    FILE *citysFile = fopen(CITYS_FILE, "a");
    int CurrentCitys;
    CurrentCitys = GetNumberOfCitys();

    if(CurrentCitys == 30)
        return 0;

    if(SearchCity(city_name) != 0)
        return 0;

    if(citysFile == NULL)
        return 0;

    fprintf(citysFile, "%s\n", city_name);
    fclose(citysFile);
    return 1;
}



int DeleteCity(char *city_name)
{
    int matchLine = SearchCity(city_name), status;
    char tmp[256];

    if(matchLine > 0)
    {
        FILE *citysFile = fopen(CITYS_FILE, "r");
        FILE *citysFileTMP = fopen("configs/cidades_temp.txt", "a");
        while(fgets(tmp, sizeof(tmp),citysFile) != NULL)
        {
            if (!strstr(tmp, city_name))
            {
                fprintf(citysFileTMP, "%s", tmp);
            }
        }
        fclose(citysFile);
        fclose(citysFileTMP);
        unlink(CITYS_FILE);
        status = rename("configs/cidades_temp.txt" , CITYS_FILE);
        return 1;
    }
    return 0;
}













