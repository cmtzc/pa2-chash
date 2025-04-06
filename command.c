#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "utilities.h"

// Helper function to trim whitespace from a string
static char* trim(char* str) {
    while (isspace((unsigned char)*str)) str++;
    
    if (*str == 0) return str;
    
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    
    end[1] = '\0';
    return str;
}

// Helper function to parse a line into a Command structure
static int parse_line(char* line, command_t* cmd) {
    char* token = strtok(line, ",");
    if (token == NULL) return -1;
    
    token = trim(token);
    
    if (strcmp(token, "insert") == 0) {
        cmd->type = CMD_INSERT;
        
        // Get name
        token = strtok(NULL, ",");
        if (token == NULL) return -1;
        strncpy(cmd->name, trim(token), sizeof(cmd->name) - 1);
        cmd->name[sizeof(cmd->name) - 1] = '\0';
        
        // Get salary
        token = strtok(NULL, ",");
        if (token == NULL) return -1;
        cmd->value = (uint32_t)atoi(trim(token));
    }
    else if (strcmp(token, "delete") == 0) {
        cmd->type = CMD_DELETE;
        
        // Get name
        token = strtok(NULL, ",");
        if (token == NULL) return -1;
        strncpy(cmd->name, trim(token), sizeof(cmd->name) - 1);
        cmd->name[sizeof(cmd->name) - 1] = '\0';
        
        cmd->value = 0;
    }
    else if (strcmp(token, "search") == 0) {
        cmd->type = CMD_SEARCH;
        
        // Get name
        token = strtok(NULL, ",");
        if (token == NULL) return -1;
        strncpy(cmd->name, trim(token), sizeof(cmd->name) - 1);
        cmd->name[sizeof(cmd->name) - 1] = '\0';
        
        cmd->value = 0;
    }
    else if (strcmp(token, "print") == 0) {
        cmd->type = CMD_PRINT;
        cmd->name[0] = '\0';
        cmd->value = 0;
    }
    else if (strcmp(token, "threads") == 0) {
        cmd->type = CMD_THREADS;
        
        // Get thread count (we'll ignore the second parameter)
        token = strtok(NULL, ",");
        if (token == NULL) return -1;
        cmd->value = (uint32_t)atoi(trim(token));
        
        cmd->name[0] = '\0';
    }
    else {
        cmd->type = CMD_OTHER;
        return -1;
    }
    
    return 0;
}

// Main function to parse commands from file
int parse_commands(const char* filename, command_t** commands) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open commands file");
        return -1;
    }
    
    char line[256];
    int count = 0;
    int capacity = 10;
    command_t* cmds = malloc(capacity * sizeof(command_t));
    if (cmds == NULL) {
        fclose(file);
        return -1;
    }
    
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines and comments
        if (line[0] == '\n' || line[0] == '#') continue;
        
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';
        
        // Resize array if needed
        if (count >= capacity) {
            capacity *= 2;
            command_t* temp = realloc(cmds, capacity * sizeof(command_t));
            if (temp == NULL) {
                free(cmds);
                fclose(file);
                return -1;
            }
            cmds = temp;
        }
        
        // Parse the line
        if (parse_line(line, &cmds[count]) == 0) {
            count++;
        }
    }
    
    fclose(file);
    
    *commands = cmds;
    return count;
}

// Function to execute a command on a hash table
void execute_command(hashTable *ht, command_t *cmd) {
    switch (cmd->type) {
        case CMD_INSERT:
            insert(ht, cmd->name, cmd->value);
            break;
        case CMD_DELETE:
            delete(ht, cmd->name);
            break;
        case CMD_SEARCH: {
            // Log the search operation
            fprintf(ht->output_file, "%lld: SEARCH,%s\n", get_timestamp(), cmd->name);
            
            // Perform the search
            hashRecord *record = search(ht, cmd->name);
            
            if (record != NULL) {
                // Print the found record
                fprintf(ht->output_file, "%u,%s,%u\n", record->hash, record->name, record->salary);
            } else {
                // Record not found
                fprintf(ht->output_file, "%lld: SEARCH: NOT FOUND\n", get_timestamp());
            }
            break;
        }
        case CMD_PRINT:
            print_table(ht);
            break;
        default:
            // Unknown command or THREADS command already handled
            break;
    }
}

// Thread worker function
void* command_worker(void *arg) {
    thread_arg_t *thread_arg = (thread_arg_t*)arg;
    execute_command(thread_arg->table, &thread_arg->cmd);
    free(thread_arg);
    return NULL;
}