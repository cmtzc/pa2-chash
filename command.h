#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include "crud.h"  // Include for hashTable definition

// Define command types
typedef enum {
    CMD_THREADS,
    CMD_INSERT,
    CMD_DELETE,
    CMD_SEARCH,
    CMD_PRINT,
    CMD_OTHER
} command_type_t;

// Command structure to hold parsed command data
typedef struct {
    command_type_t type;
    char name[50];        // For insert, delete, search
    uint32_t value;       // For insert (salary) or threads (count)
} command_t;

// Thread argument structure
typedef struct {
    hashTable *table;
    command_t cmd;
} thread_arg_t;

// Function declarations
extern command_t parse_command_line(char *line);
extern command_t* read_commands(const char *filename, int *num_commands);
extern void* command_worker(void *arg);
extern int parse_commands(const char* filename, command_t** commands);

#endif /* COMMANDS_H */

