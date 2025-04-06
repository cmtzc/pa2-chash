#include "rwlock.h"
#include "utilities.h"
#include <stdlib.h>

void rwlock_init(rwlock_t *rw, FILE* output_file) {
    pthread_mutex_init(&rw->lock, NULL);
    pthread_cond_init(&rw->read_cv, NULL);
    pthread_cond_init(&rw->write_cv, NULL);
    rw->readers = 0;
    rw->writers = 0;
    rw->waiting_writers = 0;
    rw->output_file = output_file;
}

void rwlock_destroy(rwlock_t *rw) {
    pthread_mutex_destroy(&rw->lock);
    pthread_cond_destroy(&rw->read_cv);
    pthread_cond_destroy(&rw->write_cv);
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    
    // Wait while there are active or waiting writers
    while (rw->writers > 0 || rw->waiting_writers > 0) {
        pthread_cond_wait(&rw->read_cv, &rw->lock);
    }
    
    // Log the attempt to acquire read lock
    fprintf(rw->output_file, "%lld: READ LOCK ACQUIRED\n", get_timestamp());
    
    
    rw->readers++;
    pthread_mutex_unlock(&rw->lock);
}

void rwlock_release_readlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    
    rw->readers--;
    // Log the release of read lock
    fprintf(rw->output_file, "%lld: READ LOCK RELEASED\n", get_timestamp());
    
    // If this is the last reader, signal to waiting writers
    if (rw->readers == 0) {
        pthread_cond_signal(&rw->write_cv);
    }
    
    pthread_mutex_unlock(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    
    // Increment waiting writers counter
    rw->waiting_writers++;
    
    // Wait until no readers and no active writers
    while (rw->readers > 0 || rw->writers > 0) {
        pthread_cond_wait(&rw->write_cv, &rw->lock);
    }
    
    // Log the acquisition of write lock
    fprintf(rw->output_file, "%lld: WRITE LOCK ACQUIRED\n", get_timestamp());
    
    // Not waiting anymore, now active
    rw->waiting_writers--;
    rw->writers++;
    
    pthread_mutex_unlock(&rw->lock);
}

void rwlock_release_writelock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->lock);
    
    rw->writers--;
    // Log the release of write lock
    fprintf(rw->output_file, "%lld: WRITE LOCK RELEASED\n", get_timestamp());
    
    // Writers have preference to avoid starvation
    if (rw->waiting_writers > 0) {
        pthread_cond_signal(&rw->write_cv);
    } else {
        // No waiting writers, wake up all readers
        pthread_cond_broadcast(&rw->read_cv);
    }
    
    pthread_mutex_unlock(&rw->lock);
}
