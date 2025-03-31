#include <string.h>
#include "crud.h"

uint32_t jenkins_one_at_a_time_hash(const char *key)
{
    size_t len = strlen(key);
    size_t i = 0;
    uint32_t hash = 0;
    while (i != len)
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

void insert(const char *name, uint32_t salary)
{
}

extern void delete(const char *name)
{
}

// extern hashRecord *search(const char *name)
// {
// }
