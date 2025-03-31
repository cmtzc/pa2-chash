#include <stdlib.h>
#include <string.h>
// #include <sys/time.h> // for UNIX
#include <sys/timeb.h>
#include "utilities.h"

// function extracting letters from text file
char *getLetters(FILE *fp, int n)
{
    char *plain = malloc(sizeof(char) * 10000 + 1);

    int x;
    int len = 0;
    while ((x = fgetc(fp)) != EOF)
    {
        if ((x >= 65 && x <= 90))
            x += 32;
        if ((x >= 97 && x <= 122))
        {
            plain[len] = (char)x;
            len++;
        }
    }

    while (len % n != 0)
    {
        plain[len] = 'x';
        len++;
    }
    // adding null terminator
    plain[len] = '\0';

    return plain;
}

// function prints out given plain/ciphertext according to criteria
void printText(char *s, FILE *op)
{
    for (int i = 0; i < strlen(s); i++)
    {
        if ((i % 80 == 0) && i != 0)
            printf("\n");
        fprintf(op, "%c", s[i]);
    }
}

// function returning # of characters in text file
int getFileSize(FILE *fp)
{
    int sz = 0;
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    rewind(fp);
    return sz;
}
// function returning the current time in microseconds
long long getTime()
{
    struct timeb tb;
    ftime(&tb);
    return (long long) tb.time * 1000LL + (tb.millitm);
}

// INCLUDE sys/time.h when testing on Eustis
// long long getTime()
// {
//     struct timeval tv;
//     assert(gettimeofday(&tv,NULL) != 0);
//     return tv.tv_sec * 1000000 + tv.tv_usec;
// }
