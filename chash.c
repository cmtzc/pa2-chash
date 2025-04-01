#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "crud.h"
#include "command.h"
#include "utilities.h"



int main(void)
{
    // FILE *fp = fopen("commands.txt", "r");
    // assert(fp != 0);

    char output[] = "output.txt";
    FILE *op = fopen(output, "w");
    assert(op != 0);

    // char command[50];
    // char line[50];
    // int threadCount, salary;

    // fgets(line, sizeof(line), fp);
    // assert(line != NULL);
    // if (sscanf(line, "%49[^,], %d, %d", command, &threadCount, &salary) == 3)
    //     fprintf(op, "Time: %lld\nCommand: %s\nThread Count: %d\nSalary: %d\n", getTime(), command, threadCount, salary);


    // fclose(fp);
    // fclose(op);

    command_t *commands = NULL;
    int num_commands = parse_commands("commands.txt", &commands);

    if (num_commands < 0) {
        fprintf(stderr, "Error parsing commands \n");
        return EXIT_FAILURE;
    }

    output_commands(op, commands, num_commands);

    free(commands);
    fclose(op);

    return EXIT_SUCCESS;
}
