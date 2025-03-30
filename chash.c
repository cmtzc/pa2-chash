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

    char *plain = getLetters(fp, getFileSize(fp));
    printText(plain);


    fclose(fp);

    return EXIT_SUCCESS;
}
