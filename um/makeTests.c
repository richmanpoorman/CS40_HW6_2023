

/*
 * makeTests.c
 *
 * Functions to generate UM unit tests. 
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */

#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <bitpack.h>
#include <seq.h>

#include "makeTests.h"
/* Note: Will use the given .um implementation to create the answer keys */

#define opCodeSize 4
#define registerSize 3
#define opCodeLsb (32 - 4)
#define raLsb 6
#define rbLsb 3
#define rcLsb 0

#define loadRegisterLsb (32 - 7)
#define loadValueSize (32 - 7)

/* Functions that return the two instruction types */

typedef enum OP_CODE {
        CMOV        = 0 , 
        SEGLOAD     = 1 , 
        SEGSTORE    = 2 , 
        ADD         = 3 , 
        MUL         = 4 , 
        DIV         = 5 ,
        NAND        = 6 , 
        HALT        = 7 , 
        MAPSEG      = 8 , 
        UNMAPSEG    = 9 , 
        OUT         = 10, 
        IN          = 11, 
        LOADPROGRAM = 12, 
        LOADVALUE   = 13
} OP_CODE;

static inline Instruction three_register(OP_CODE op, int ra, int rb, int rc);
static inline Instruction three_register(OP_CODE op, int ra, int rb, int rc)
{
        Instruction instruction = 0;
        instruction = Bitpack_newu(instruction, opCodeSize, opCodeLsb, op);
        instruction = Bitpack_newu(instruction, registerSize, raLsb, ra);
        instruction = Bitpack_newu(instruction, registerSize, rbLsb, rb);
        instruction = Bitpack_newu(instruction, registerSize, rcLsb, rc);

        return instruction;

}

/* Functions for working with streams */

static inline void append(Seq_T stream, Instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Wrapper functions for each of the instructions */

void cmove(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc)
{
        append(stream, three_register(CMOV, ra, rb, rc));
}
void segLoad(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc)
{
        append(stream, three_register(SEGLOAD, ra, rb, rc));
}
void segStore(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc)
{
        append(stream, three_register(SEGSTORE, ra, rb, rc));
}
void add(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc)
{
        append(stream, three_register(ADD, ra, rb, rc));
}
void mult(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc)
{
        append(stream, three_register(MUL, ra, rb, rc));
}
void divide(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc)
{
        append(stream, three_register(DIV, ra, rb, rc));
}
void nand(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc)
{
        append(stream, three_register(NAND, ra, rb, rc));
}
void halt(Seq_T stream)
{
        append(stream, three_register(HALT, 0, 0, 0));
}
void mapSeg(Seq_T stream, REGISTER rb, REGISTER rc)
{
        append(stream, three_register(MAPSEG, 0, rb, rc));
}
void unmapSeg(Seq_T stream, REGISTER rc)
{
        append(stream, three_register(UNMAPSEG, 0, 0, rc));
}
void printOut(Seq_T stream, REGISTER rc)
{
        append(stream, three_register(OUT, 0, 0, rc));
}
void readIn(Seq_T stream, REGISTER rc)
{
        append(stream, three_register(IN, 0, 0, rc));
}
void loadProgram(Seq_T stream, REGISTER rb, REGISTER rc)
{
        append(stream, three_register(LOADPROGRAM, 0, rb, rc));
}
void loadValue(Seq_T stream, REGISTER rl, unsigned value)
{
        Instruction instruction = 0;
        instruction = Bitpack_newu(instruction, opCodeSize, opCodeLsb, 
                                   LOADVALUE);
        instruction = Bitpack_newu(instruction, registerSize, loadRegisterLsb, 
                                   rl);
        instruction = Bitpack_newu(instruction, loadValueSize, 0, value);
        append(stream, instruction);
}

#undef opCodeSize
#undef registerSize
#undef opCodeLsb
#undef raLsb 
#undef rbLsb 
#undef rcLsb 
#undef loadRegisterLsb
#undef loadValueSize 