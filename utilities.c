#include <stdlib.h>
#include <string.h>
#include "utilities.h"

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
void printText(char *s)
{
    for (int i = 0; i < strlen(s); i++)
    {
        if ((i % 80 == 0) && i != 0)
            printf("\n");
        fprintf(stdout, "%c", s[i]);
    }
}

int getFileSize(FILE *fp)
{
    int sz = 0;
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    rewind(fp);
    return sz;
}

uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length)
{
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length)
    {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

