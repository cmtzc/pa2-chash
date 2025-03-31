#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

// Function to free allocated commands
void free_commands(command_t* commands) {
    free(commands);
}
