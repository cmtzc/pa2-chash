#ifndef CRUD_H
#define CRUD_H

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
    size_t numElements, capacity;
    hashRecord *arr;
} hashTable;

// slightly modified hash function based on Wikipedia version
extern uint32_t jenkins_one_at_a_time_hash(const char *key);

// function initializing hash table
extern hashTable* init_table();

// function destroying hash table
extern void destroy_table(hashTable *ht);

// function searching for key (name) in hash table
extern uint32_t search(const char *name);

// function to insert in a hash table
extern void insert(hashTable *ht, const char *name, uint32_t salary);

// function to delete item from hash table
extern void delete(const char *name);

#endif