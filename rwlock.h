#ifndef RWLOCK_H
#define RWLOCK_H

#include <pthread.h>
#include <stdio.h>

typedef struct rwlock {
    pthread_mutex_t lock;       // Basic lock for the entire structure
    pthread_cond_t read_cv;     // Readers wait on this CV
    pthread_cond_t write_cv;    // Writers wait on this CV
    int readers;                // Count of active readers
    int writers;                // Count of active writers (0 or 1)
    int waiting_writers;        // Count of waiting writers
    FILE* output_file;          // File for logging lock operations
    void* table;                // Pointer to the containing hash table
} rwlock_t;

// Initialize the reader-writer lock
extern void rwlock_init(rwlock_t *rw, FILE* output_file);

// Destroy the reader-writer lock
extern void rwlock_destroy(rwlock_t *rw);

// Acquire a read lock
extern void rwlock_acquire_readlock(rwlock_t *rw);

// Release a read lock
extern void rwlock_release_readlock(rwlock_t *rw);

// Acquire a write lock
extern void rwlock_acquire_writelock(rwlock_t *rw);

// Release a write lock
extern void rwlock_release_writelock(rwlock_t *rw);

#endif /* RWLOCK_H */