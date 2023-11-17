#include "CPU.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[])
{
        FILE *input   = stdin;
        FILE *output  = stdout;
        FILE *program = stdin;

        assert(argc == 2);
        program = fopen(argv[1], "r");
        assert(program != NULL);

        runProgram(input, output, program);

        fclose(program);

}