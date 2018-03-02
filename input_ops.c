#define __USE_XOPEN
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *substring(char *string, int position, int length) 
{
    char *pointer = malloc(length+1);
    if (pointer == NULL)
    {
        printf("Unable to allocate memory.\n");
        exit(1);
    }

    int c;
    for (c = 0 ; c < length ; c++)
    {
        *(pointer+c) = *(string+position-1);      
        string++;   
    }
    *(pointer+c) = '\0';

    return pointer;
}

time_t toTime (char *time_details)
{
    struct tm tm;
    strptime(time_details, "%H:%M:%S", &tm);
    time_t t = mktime(&tm);
    return t;
}

char* getParameter(char* input, char* parameter)
{
    int c = 0;
    while (*(input+c) != '\0' && *(input+c) != '+' && *(input+c) != '\n')
    {
        *(parameter+c) = *(input+c);
        c++;
    }

    return (input+c); //returns pointer to last untouched symbol
}