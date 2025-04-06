#include <stdio.h>
#include "command.h"

// function reads in file and extracts the lowercase letters (converting if necessary)
extern char *getLetters(FILE *fp, int n);

// function prints out given plain/ciphertext according to criteria
extern void printText(char *s, FILE *op);

// function returning # of characters in text file
extern int getFileSize(FILE *fp);

// function returning the current time in microseconds
extern long long get_timestamp();

// function outputs commands to text file
extern void output_commands(FILE *op, command_t *commands, size_t n);

