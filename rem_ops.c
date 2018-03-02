char* getParameter(char* input, int parIndex)
{
    char* para_s;

    int c = 0;
    while (*(input+c) != '\0' && *(input+c) != '+')
    {
        *(para_s+c) = *(input+c);
        c++;
    }

    return para_s;
}

// Command validation at arrival
/*
char* name;// = substring

int c = 0;
while (*(sendbuffer+4) != '\0' && *(insendbufferput+c) != '+')
{
    *(name+c) = *(sendbuffer+c);
    c++;
}
*/