#include "Memory.h"
#include <stdint.h>
#include <mem.h>
#include <stdbool.h>
#include <stdio.h>
#include <seq.h>

#define MAX_VALUE 0xffffffff
#define instructionLsb 28
#define instructionSize 4
#define raLsb 6
#define rbLsb 3
#define rcLsb 0
#define rLoad 25
#define registerIDSize 3
#define loadLsb 0
#define loadSize 25

typedef struct CPU_State {
        Mem mem;
        FILE *input;
        FILE *output;
        uint32_t registers[8];
        uint32_t programCounter;
        bool isRunning;
} *CPU_State;

typedef enum INSTRUCTION_TYPE {
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
} INSTRUCTION_TYPE;

void runProgram(FILE *input, FILE *output, FILE *program);

CPU_State CPU_new(FILE *input, FILE *output);
void initializeProgram(FILE *program);

uint32_t getInstruction(CPU_State state);
void executeFunction(CPU_State state);

void CPU_cmove(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc);
void CPU_segLoad(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc);
void CPU_segStore(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc);
void CPU_add(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc);
void CPU_mult(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc);
void CPU_div(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc);
void CPU_nand(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc);
void CPU_halt(CPU_State);
void CPU_mapSeg(CPU_State state, uint32_t rb, uint32_t rc);
void CPU_unmapSeg(CPU_State state, uint32_t rc);
void CPU_printOut(CPU_State state, uint32_t rc);
void CPU_readIn(CPU_State state, uint32_t rc);
void CPU_loadProgram(CPU_State state, uint32_t rb, uint32_t rc);
void CPU_loadValue(CPU_State state, uint32_t rL, uint32_t val);


void runProgram(FILE *input, FILE *output, FILE *program)
{
        CPU_State state = CPU_new(input, output);
        initializeProgram(state, program);

        while (state -> isRunning) {
                executeFunction(state);
        }
}

CPU_State CPU_new(FILE *input, FILE *output) {
        CPU_State cpu = ALLOC(sizeof(*cpu));
        cpu -> mem = Mem_new();
        cpu -> input = input;
        cpu -> output = output;
        cpu -> registers = CALLOC(8, sizeof(uint32_t));
        cpu -> programCounter = 0;
        cpu -> isRunning = true;

        return cpu;
}


void initializeProgram(CPU_State state, FILE *program)
{
        
        Seq_T instructions = Seq_new(1024);
        while (!feof(program)) {
                uint32_t instruction = 0;
                for (int i = 3; i >= 0; i++) {
                        assert(!feof(program) && !ferror(program));
                        char byte = fgetc(program);
                        instruction = Bitpack_newu(instruction, i * 8, 8);
                }
                Seq_addhi(instructions, (void *)(uintptr_t)instruction);
        }

        Segment program = Segment_new(Seq_length(instructions));

        int index = 0;
        while (Seq_length(instructions) > 0)
        {
                uint32_t instruction = 
                        (uint32_t)(uintptr_t) Seq_remlo(instructions);
                
                Segment_setWord(program, index, instruction);
                index++;
        }

        Mem mem = state -> mem;
        Mem_setSegment(mem, 0, program);
}


void executeFunction(CPU_State state)
{
        if (!(state -> isRunning)) {
                return;
        }
        uint32_t instruction = getInstruction(state);
        assert(instruction != MAX_VALUE);

        INSTRUCTION_TYPE instructionType = Bitpack_getu(instruction, 
                                                        instructionLsb, 
                                                        instructionSize);

        uint32_t ra = Bitpack_getu(instruction, raLsb, registerIDSize);
        uint32_t rb = Bitpack_getu(instruction, rbLsb, registerIDSize);
        uint32_t rc = Bitpack_getu(instruction, rcLsb, registerIDSize);
        uint32_t rL = Bitpack_getu(instruction, rLoad, registerIDSize);
        uint32_t loadVal = Bitpack_getu(instruction, loadLsb, loadSize);

        switch(instructionType) {
                case CMOV:
                CPU_cmove(state, ra, rb, rc);
                break;
                case SEGLOAD:
                CPU_segLoad(state, ra, rb, rc);
                break;
                case SEGSTORE:
                CPU_segStore(state, ra, rb, rc);
                break;
                case ADD:
                CPU_add(state, ra, rb, rc);
                break;
                case MUL:
                CPU_mult(state, ra, rb, rc);
                break;
                case DIV:
                CPU_div(state, ra, rb, rc);
                break;
                case NAND:
                CPU_nand(state, ra, rb, rc);
                break;
                case HALT:
                CPU_halt();
                break;
                case MAPSEG:
                CPU_mapSeg(state, rb, rc);
                break;
                case UNMAPSEG:
                CPU_unmapSeg(state, rc);
                break;
                case OUT:
                CPU_printOut(state, rc);
                break;
                case IN:
                CPU_readIn(state, rc);
                break;
                case LOADPROGRAM:
                CPU_loadProgram(state, rb, rc);
                break;
                case LOADVALUE:
                CPU_loadValue(state, rL)
                break;
        }

        if (!Mem_isInRange(mem, 0, state -> programCounter)) {
                state -> isRunning = false;
        }
}

uint32_t getInstruction(CPU_State state)
{
        Mem mem = state -> mem;
        
        if (!Mem_isInRange(mem, segID, wordID)) {
                return FINISHED_INSTRUCTION;
        }
        uint32_t instruction = Mem_getWord(mem, 0, state -> programCounter);
        state -> programCounter++;
        return instruction;
}

void CPU_cmove(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        if (registers[rc] == 0) {
                return;
        }
        registers[ra] = registers[rb];
}

void CPU_segLoad(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        Mem      mem           = state -> mem;
        registers[ra] = Mem_getWord(mem, registers[rb], registers[rc]);
}

void CPU_segStore(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        Mem_setWord(mem, regiseters[ra], registers[rb], registers[rc]);
}

void CPU_add(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        registers[ra] = registers[rb] + registers[rc];
}

void CPU_mult(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        registers[ra] = registers[rb] * registers[rc];
}

void CPU_div(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        assert(registers[rc] != 0)
        registers[ra] = registers[rb] / registers[rc];
}

void CPU_nand(CPU_State state, uint32_t ra, uint32_t rb, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        registers[ra] = ~(registers[rb] & registers[rc]);

}

void CPU_halt(CPU_State state)
{
        state -> isRunning = false;
}

void CPU_mapSeg(CPU_State state, uint32_t rb, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        Mem      mem           = state -> mem;

        registers[rb] = Mem_mapNew(mem, registers[rc]);
}

void CPU_unmapSeg(CPU_State state, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        Mem      mem           = state -> mem;

        Mem_mapFree(mem, registers[rc]);
}

void CPU_printOut(CPU_State state, uint32_t rc)
{
        uint32_t  registers [8] = state -> registers;
        FILE     *output        = state -> output;

        char printChar = registers[rc];
        fprintf(output, "%c", printChar);
}
void CPU_readIn(CPU_State state, uint32_t rc)
{
        uint32_t  registers [8] = state -> registers;
        FILE     *input         = state -> input;

        if (feof(input)) {
                registers[rc] = MAX_VALUE;
                return;
        }

        registers[rc] = fgetc(input);
}
void CPU_loadProgram(CPU_State state, uint32_t rb, uint32_t rc)
{
        uint32_t registers [8] = state -> registers;
        Mem      mem           = state -> mem;

        state -> programCounter = registers[rc];

        if (registers[rb] == 0) {
                return;
        }

        Segment seg  = Mem_getSegment(mem, registers[rb]);
        Segment copy = Segment_copy(seg);
        Mem_setSegment(mem, registers[rb], copy);
}

void CPU_loadValue(CPU_State state, uint32_t rL, uint32_t val)
{
        uint32_t registers [8] = state -> registers;
        registers[rL] = val;
}

#undef MAX_VALUE
#undef instructionLsb
#undef instructionSize
#undef raLsb 
#undef rbLsb 
#undef rcLsb 
#undef rLoad  
#undef registerIDSize 
#undef loadLsb 
#undef loadSize