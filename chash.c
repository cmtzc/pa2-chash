#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include "crud.h"
#include "command.h"
#include "utilities.h"

int main(void) {
    // Open output file
    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("Failed to open output file");
        return EXIT_FAILURE;
    }
    
    // Parse commands from file
    command_t *commands = NULL;
    int num_commands = parse_commands("commands.txt", &commands);
    
    if (num_commands <= 0) {
        fprintf(stderr, "Error parsing commands or no valid commands found\n");
        fclose(output_file);
        return EXIT_FAILURE;
    }
    
    // Verify we have a threads command as the first command
    if (commands[0].type != CMD_THREADS) {
        fprintf(stderr, "First command must be 'threads'\n");
        free(commands);
        fclose(output_file);
        return EXIT_FAILURE;
    }
    
    // Get thread count from the first command
    int thread_count = commands[0].value;
    
    // Initialize the hash table
    hashTable *ht = init_table(output_file);
    if (ht == NULL) {
        fprintf(stderr, "Failed to initialize hash table\n");
        free(commands);
        fclose(output_file);
        return EXIT_FAILURE;
    }

    fprintf(output_file, "Running %d threads...\n", thread_count);
    
    // Create threads for each command after the threads command
    pthread_t *threads = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Failed to allocate memory for threads\n");
        destroy_table(ht);
        free(commands);
        fclose(output_file);
        return EXIT_FAILURE;
    }
    
    // Create and start threads
    for (int i = 0; i < thread_count && i + 1 < num_commands; i++) {
        thread_arg_t *arg = (thread_arg_t*)malloc(sizeof(thread_arg_t));
        if (arg == NULL) {
            fprintf(stderr, "Failed to allocate memory for thread arguments\n");
            // Clean up created threads
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            destroy_table(ht);
            free(threads);
            free(commands);
            fclose(output_file);
            return EXIT_FAILURE;
        }
        
        // Set up thread argument
        arg->table = ht;
        arg->cmd = commands[i + 1]; // Skip the first threads command
        
        
        // Create thread
        if (pthread_create(&threads[i], NULL, command_worker, arg) != 0) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            free(arg);
            // Clean up created threads
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            destroy_table(ht);
            free(threads);
            free(commands);
            fclose(output_file);
            return EXIT_FAILURE;
        }
    }


    // Wait for all threads to complete
    for (int i = 0; i < thread_count && i + 1 < num_commands; i++) {
        pthread_join(threads[i], NULL);
    }
    
    fprintf(output_file, "Finished all threads.\n\n");
    
    // Print final summary information
    fprintf(output_file, "Number of lock acquisitions: %d\n", ht->lock_acquires);
    fprintf(output_file, "Number of lock releases: %d\n", ht->lock_releases);
    
    // Disable lock counting for the final print
    // int saved_acquires = ht->lock_acquires;
    // int saved_releases = ht->lock_releases;
    // ht->rwlock.table = NULL;  // Temporarily disable counting
    
    // Print final hash table contents in sorted order (by hash)
    // The list should already be sorted by hash because of our insertion logic
    // rwlock_acquire_readlock(&ht->rwlock);

    print_table(ht);
    
    // hashRecord *current = ht->head;
    // while (current != NULL) {
    //     fprintf(output_file, "%u,%s,%u\n", current->hash, current->name, current->salary);
    //     current = current->next;
    // }
    
    // rwlock_release_readlock(&ht->rwlock);
    
    // Restore lock counting if needed
    // ht->rwlock.table = ht;
    // ht->lock_acquires = saved_acquires;
    // ht->lock_releases = saved_releases;
    
    // Clean up resources
    destroy_table(ht);
    free(threads);
    free(commands);
    fclose(output_file);
    
    return EXIT_SUCCESS;
}