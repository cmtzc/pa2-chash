#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "crud.h"
#include "utilities.h"

int main(void)
{
    FILE *fp = fopen("commands.txt", "r");
    assert(fp != 0);

    char output[] = "output.txt";
    FILE *op = fopen(output, "w");
    assert(op != 0);

    char command[50];
    char line[50];
    int threadCount, salary;

    fgets(line, sizeof(line), fp);
    assert(line != NULL);
    if (sscanf(line, "%49[^,], %d, %d", command, &threadCount, &salary) == 3)
        fprintf(op, "Time: %lld\nCommand: %s\nThread Count: %d\nSalary: %d\n", getTime(), command, threadCount, salary);

    // char *plain = getLetters(fp, getFileSize(fp));
    // printText(plain, op);


    fclose(fp);
    fclose(op);
    return EXIT_SUCCESS;
}
