#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "crud.h"

#define INITIAL_CAPACITY 32

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

hashTable *init_table()
{
    // allocating space
    hashTable *t = malloc(sizeof(hashTable));

    assert(t != NULL);
    if (t == NULL)
        return NULL;

    t->numElements = 0;
    t->capacity = INITIAL_CAPACITY;

    // allocate
    t->arr = calloc(t->capacity, sizeof(hashRecord));
    assert(t->arr != NULL);
    if (t->arr == NULL)
    {
        free(t);
        return NULL;
    }
    return t;
}

// probably not right, check
void destroy_table(hashTable *ht)
{
    assert(ht != NULL);
    while (ht->arr != NULL)
    {
        free(ht->arr);
        ht->arr = ht->arr->next;
    }
    free(ht);
}

uint32_t search(const char *name)
{

    uint32_t hash = jenkins_one_at_a_time_hash(name);
    

    return EXIT_SUCCESS;
}

void insert(hashTable *ht, const char *name, uint32_t salary)
{
    assert(name != NULL);
    if (name == NULL)
        return;

    uint32_t hash = jenkins_one_at_a_time_hash(name);
    // size_t index = (size_t)(hash & (uint64_t)(ht->capacity - 1));

    while (ht->arr->next != NULL)
    {
        if (ht->arr->hash == hash)
        {
            ht->arr->salary = salary;
            return;
        }
        else
        {

        }
    }
}

void delete(const char *name)
{
}
