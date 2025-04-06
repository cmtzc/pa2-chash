#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "crud.h"
#include "utilities.h"

// Implementation of Jenkins one-at-a-time hash function
uint32_t jenkins_one_at_a_time_hash(const char *key) {
    size_t len = strlen(key);
    uint32_t hash = 0;
    
    for (size_t i = 0; i < len; i++) {
        hash += key[i];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    
    return hash;
}

// Initialize the hash table
hashTable* init_table(FILE *output_file) {
    hashTable *ht = (hashTable*)malloc(sizeof(hashTable));
    if (ht == NULL) return NULL;
    
    // Initialize the hash table fields
    ht->head = NULL;
    
    // Initialize reader-writer lock
    rwlock_init(&ht->rwlock, output_file);
    
    // Set the table pointer in the rwlock to point back to the hash table
    ht->rwlock.table = ht;
    
    // Initialize mutex and condition variable for insert/delete synchronization
    pthread_mutex_init(&ht->insert_mutex, NULL);
    pthread_cond_init(&ht->delete_cv, NULL);
    
    // Initialize counters
    ht->pending_inserts = 0;
    ht->completed_inserts = 0;
    ht->lock_acquires = 0;
    ht->lock_releases = 0;
    
    // Store output file
    ht->output_file = output_file;
    
    return ht;
}

// Clean up and destroy the hash table
void destroy_table(hashTable *ht) {
    if (ht == NULL) return;
    
    // Free all nodes in the linked list
    hashRecord *current = ht->head;
    while (current != NULL) {
        hashRecord *next = current->next;
        free(current);
        current = next;
    }
    
    // Destroy locks and condition variables
    rwlock_destroy(&ht->rwlock);
    pthread_mutex_destroy(&ht->insert_mutex);
    pthread_cond_destroy(&ht->delete_cv);
    
    // Free the hash table itself
    free(ht);
}

// Search for a record by name
hashRecord* search(hashTable *ht, const char *name) {
    if (ht == NULL || name == NULL) return NULL;
    
    // Compute the hash
    uint32_t hash = jenkins_one_at_a_time_hash(name);
    
    // Acquire read lock
    rwlock_acquire_readlock(&ht->rwlock);
    track_lock_acquisition(ht);
    // Search the linked list
    hashRecord *current = ht->head;
    while (current != NULL) {
        if (current->hash == hash && strcmp(current->name, name) == 0) {
            // Found the record
            rwlock_release_readlock(&ht->rwlock);
            track_lock_release(ht);
            return current;
        }
        current = current->next;
    }
    
    // Release read lock
    rwlock_release_readlock(&ht->rwlock);
    track_lock_release(ht);
    
    // Record not found
    return NULL;
}

// Insert or update a record
void insert(hashTable *ht, const char *name, uint32_t salary) {
    if (ht == NULL || name == NULL) return;
    
    // Register this insert operation
    register_insert(ht);
    
    // Compute the hash
    uint32_t hash = jenkins_one_at_a_time_hash(name);
    
    // Log the insert operation
    fprintf(ht->output_file, "%lld: INSERT,%ud,%s,%u\n", get_timestamp(), hash, name, salary);
    
    // Acquire write lock
    rwlock_acquire_writelock(&ht->rwlock);
    track_lock_acquisition(ht);
    
    // Check if the key already exists
    hashRecord *current = ht->head;
    // hashRecord *prev = NULL;
    
    while (current != NULL) {
        if (current->hash == hash && strcmp(current->name, name) == 0) {
            // Update existing record
            current->salary = salary;
            rwlock_release_writelock(&ht->rwlock);
            track_lock_release(ht);
            complete_insert(ht);
            return;
        }
        // prev = current;
        current = current->next;
    }
    
    // Create a new record
    hashRecord *new_record = (hashRecord*)malloc(sizeof(hashRecord));
    if (new_record == NULL) {
        rwlock_release_writelock(&ht->rwlock);
        track_lock_release(ht);
        complete_insert(ht);
        return;
    }
    
    // Initialize the new record
    new_record->hash = hash;
    strncpy(new_record->name, name, sizeof(new_record->name) - 1);
    new_record->name[sizeof(new_record->name) - 1] = '\0';
    new_record->salary = salary;
    new_record->next = NULL;
    
    // Insert in sorted order by hash value
    if (ht->head == NULL) {
        // First element
        ht->head = new_record;
    } else if (hash < ht->head->hash) {
        // Insert at the beginning if hash is smaller than head
        new_record->next = ht->head;
        ht->head = new_record;
    } else {
        // Find the correct position to insert
        current = ht->head;
        while (current->next != NULL && current->next->hash < hash) {
            current = current->next;
        }
        // Insert after current
        new_record->next = current->next;
        current->next = new_record;
    }
    
    // Release write lock
    rwlock_release_writelock(&ht->rwlock);
    track_lock_release(ht);
    
    // Complete the insert operation
    complete_insert(ht);
}

// Delete a record by name
void delete(hashTable *ht, const char *name) {
    if (ht == NULL || name == NULL) return;
    
    // Wait for all pending inserts to complete
    wait_for_inserts(ht);
    
    // Compute the hash
    uint32_t hash = jenkins_one_at_a_time_hash(name);
    
    // Log the delete operation
    fprintf(ht->output_file, "%lld: DELETE,%s\n", get_timestamp(), name);
    
    // Acquire write lock
    rwlock_acquire_writelock(&ht->rwlock);
    track_lock_acquisition(ht);
    
    // Search for the node to delete
    hashRecord *current = ht->head;
    hashRecord *prev = NULL;
    
    while (current != NULL) {
        if (current->hash == hash && strcmp(current->name, name) == 0) {
            // Found the node to delete
            if (prev == NULL) {
                // Deleting the head node
                ht->head = current->next;
            } else {
                // Deleting a node in the middle or end
                prev->next = current->next;
            }
            
            // Free the node
            free(current);
            
            // Release write lock
            rwlock_release_writelock(&ht->rwlock);
            track_lock_release(ht);
            return;
        }
        
        prev = current;
        current = current->next;
    }
    
    // Node not found, release write lock
    rwlock_release_writelock(&ht->rwlock);
    track_lock_release(ht);
}

// Print the entire hash table
void print_table(hashTable *ht) {
    if (ht == NULL) return;
    
    // Log the print operation
    // fprintf(ht->output_file, "%lld,PRINT\n", get_timestamp());
    
    // Acquire read lock
    rwlock_acquire_readlock(&ht->rwlock);
    track_lock_acquisition(ht);
    
    // The list should already be sorted by hash value due to our insertion logic
    // Traverse the linked list and print records
    hashRecord *current = ht->head;
    while (current != NULL) {
        fprintf(ht->output_file, "%u,%s,%u\n", current->hash, current->name, current->salary);
        current = current->next;
    }
    
    // Release read lock
    rwlock_release_readlock(&ht->rwlock);
    track_lock_release(ht);
}

// Register an insert operation
void register_insert(hashTable *ht) {
    pthread_mutex_lock(&ht->insert_mutex);
    ht->pending_inserts++;
    pthread_mutex_unlock(&ht->insert_mutex);
}

// Complete an insert operation
void complete_insert(hashTable *ht) {
    pthread_mutex_lock(&ht->insert_mutex);
    ht->pending_inserts--;
    ht->completed_inserts++;
    
    // If no more pending inserts, signal all waiting delete operations
    if (ht->pending_inserts == 0) {
        pthread_cond_broadcast(&ht->delete_cv);
    }
    
    pthread_mutex_unlock(&ht->insert_mutex);
}

// Wait for all inserts to complete before delete
void wait_for_inserts(hashTable *ht) {
    pthread_mutex_lock(&ht->insert_mutex);
    
    // If there are pending inserts, wait
    if (ht->pending_inserts > 0) {
        fprintf(ht->output_file, "%lld: WAITING ON INSERTS\n", get_timestamp());
        pthread_cond_wait(&ht->delete_cv, &ht->insert_mutex);
        fprintf(ht->output_file, "%lld: DELETE AWAKENED\n", get_timestamp());
    }
    
    pthread_mutex_unlock(&ht->insert_mutex);
}

// Track a lock acquisition
void track_lock_acquisition(hashTable *ht) {
    if (ht) {
        ht->lock_acquires++;
    }
}

// Track a lock release
void track_lock_release(hashTable *ht) {
    if (ht) {
        ht->lock_releases++;
    }
}
