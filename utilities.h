#include <stdio.h>
#include <stdint.h>

// function reads in file and extracts the lowercase letters (converting if necessary)
extern char *getLetters(FILE *fp, int n);

// function prints out given plain/ciphertext according to criteria
extern void printText(char *s);

extern int getFileSize(FILE *fp);

extern uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length);

