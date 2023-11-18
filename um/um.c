#include "CPU.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[])
{
        

        assert(argc == 2);
        FILE *input   = stdin;
        FILE *output  = stdout;
        FILE *program = fopen(argv[1], "r");
        assert(program != NULL);

        runProgram(input, output, program);

        fclose(program);

}