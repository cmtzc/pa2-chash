#ifndef CRUD_H
#define CRUD_H

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "rwlock.h"

// Define the hash record struct as specified in the assignment
typedef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;

// Hash table structure with lock and condition variables
typedef struct {
    hashRecord *head;             // Head of the linked list
    rwlock_t rwlock;              // Reader-writer lock for thread safety
    pthread_mutex_t insert_mutex; // Mutex for tracking insert operations
    pthread_cond_t delete_cv;     // CV for delete operations to wait on
    int pending_inserts;          // Counter for pending insert operations
    int completed_inserts;        // Counter for completed insert operations
    int lock_acquires;            // Counter for lock acquisitions
    int lock_releases;            // Counter for lock releases
    FILE *output_file;            // File for logging operations
} hashTable;

// Jenkins one-at-a-time hash function
extern uint32_t jenkins_one_at_a_time_hash(const char *key);

// Initialize hash table
extern hashTable* init_table(FILE *output_file);

// Destroy hash table
extern void destroy_table(hashTable *ht);

// Search for a record by name
extern hashRecord* search(hashTable *ht, const char *name);

// Insert or update a record
extern void insert(hashTable *ht, const char *name, uint32_t salary);

// Delete a record by name
extern void delete(hashTable *ht, const char *name);

// Print the entire hash table
extern void print_table(hashTable *ht);

// Register an insert operation
extern void register_insert(hashTable *ht);

// Complete an insert operation
extern void complete_insert(hashTable *ht);

// Wait for all inserts to complete before delete
extern void wait_for_inserts(hashTable *ht);

// Track a lock acquisition
extern void track_lock_acquisition(hashTable *ht);

// Track a lock release
extern void track_lock_release(hashTable *ht);

#endif /* CRUD_H */