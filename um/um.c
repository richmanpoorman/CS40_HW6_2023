/* 
 *   Name       : um.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Driver file for our UM implementation
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include "CPU.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[])
{
        if (argc != 2) {
                fprintf(stderr, "Not given correct amount of inputs\n");
                exit(EXIT_FAILURE);
        }
        FILE *input   = stdin;
        FILE *output  = stdout;
        FILE *program = fopen(argv[1], "r");
        if (program == NULL) {
                fprintf(stderr, "Not given a accessible program file\n");
                exit(EXIT_FAILURE);
        }

        runProgram(input, output, program);

        fclose(program);

}