#include <stdio.h>
#include <stdint.h>

typedef struct hash_struct
{
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

typedef struct hashMap
{
    int numElements, capacity;
    hashRecord **arr;
} hashTable;

extern uint32_t jenkins_one_at_a_time_hash(const char *key);

extern void insert(const char *name, uint32_t salary);

extern void delete(const char *name);

extern hashRecord* search(const char *name);

