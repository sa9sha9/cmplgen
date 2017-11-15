#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

using namespace std;


int main(void) {
    double n = 130000000, p = 1;
    float f = 1/(16^2);
    cout << f << endl;

         return;
    for (int i = 1; i<n; i++) {
        p *= (i/16^24);
    }

    cout << p << endl;
}


/**

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

 */