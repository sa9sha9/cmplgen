#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"



static FILE *srcFilePointer;

void initializeScanner(char *);

int main(void) {
    char filename[] = "hoge.txt";
    initializeScanner(filename);

    return 0;
}

void initializeScanner(char *filename)
{
    FILE *fp;
    fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("%s file not open!\n", filename);
    } else {
        printf("%s file opened!\n", filename);

        srcFilePointer = fp;

    }

    fclose(fp);
}
