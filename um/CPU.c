/* 
 *   Name       : CPU.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module responsible for performs the arithmetic and
 *                logical operations of the Universal Machine; fetches,
 *                decodes, and executes instructions from memory
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include "Memory.h"
#include <stdint.h>
#include <mem.h>
#include <stdbool.h>
#include <stdio.h>
#include <seq.h>
// #include <assert.h>

#define MAX_VALUE 0xffffffff

#define loadBits 0x01ffffff
#define instructionBits 0xf0000000
#define raBits 0x000001c0
#define rbBits 0x00000038
#define rcBits 0x00000007
#define rLoadBits 0x0e000000
#define raLsb 6
#define rbLsb 3
#define rLoadLsb 25

#define CMOV         0x00000000
#define SEGLOAD      0x10000000
#define SEGSTORE     0x20000000
#define ADD          0x30000000
#define MUL          0x40000000
#define DIV          0x50000000
#define NAND         0x60000000
#define HALT         0x70000000
#define MAPSEG       0x80000000
#define UNMAPSEG     0x90000000
#define OUT          0xa0000000
#define IN           0xb0000000
#define LOADPROGRAM  0xc0000000
#define LOADVALUE    0xd0000000
/*
 * Name    : CPU_State
 * Purpose : Represents the current state of the computer at any given time
 * Notes   : Only used inside of CPU
 */
typedef struct CPU_State {
        Mem mem;
        FILE *input;
        FILE *output;
        uint32_t registers[8];
        uint32_t programCounter;
        bool isRunning;
        uint32_t mainInstructionSize;
} *CPU_State;


void runProgram(FILE *input, FILE *output, FILE *program);

CPU_State CPU_new(FILE *input, FILE *output);
void CPU_free(CPU_State *state);
void initializeProgram(CPU_State state, FILE *program);

uint32_t getInstruction(CPU_State state);
void executeFunction(CPU_State state);

 void CPU_cmove(CPU_State state, uint32_t ra, uint32_t rb, 
                             uint32_t rc);
 void CPU_segLoad(CPU_State state, uint32_t ra, uint32_t rb, 
                               uint32_t rc);
 void CPU_segStore(CPU_State state, uint32_t ra, uint32_t rb, 
                                uint32_t rc);
 void CPU_add(CPU_State state, uint32_t ra, uint32_t rb, 
                           uint32_t rc);
 void CPU_mult(CPU_State state, uint32_t ra, uint32_t rb, 
                            uint32_t rc);
 void CPU_div(CPU_State state, uint32_t ra, uint32_t rb, 
                           uint32_t rc);
 void CPU_nand(CPU_State state, uint32_t ra, uint32_t rb, 
                            uint32_t rc);
 void CPU_halt(CPU_State state);
 void CPU_mapSeg(CPU_State state, uint32_t rb, uint32_t rc);
 void CPU_unmapSeg(CPU_State state, uint32_t rc);
 void CPU_printOut(CPU_State state, uint32_t rc);
 void CPU_readIn(CPU_State state, uint32_t rc);
 void CPU_loadProgram(CPU_State state, uint32_t rb, uint32_t rc);
 void CPU_loadValue(CPU_State state, uint32_t instruction);

/*
 * Name      : runProgram
 * Purpose   : Emulates the Universal Machine with the given program and 
 *             the given input, output streams
 * Parameter : (FILE *) input   -- The input stream to read input from
 *             (FILE *) output  -- The output stream to write output to
 *             (FILE *) program -- The .um binary file with the program
 * Return    : None
 * Notes     : Runs the entire program, and only ends when it reaches the end
 *             of instructions, or the program halts
 */
void runProgram(FILE *input, FILE *output, FILE *program)
{
        CPU_State state = CPU_new(input, output);
        
        initializeProgram(state, program);
        while (state -> isRunning) {
                executeFunction(state);
        }
        
        CPU_free(&state);
}

/*
 * Name      : CPU_new
 * Purpose   : Creates the CPU state that represents a new CPU
 * Parameter : (FILE *) input   -- The input stream to read input from
 *             (FILE *) output  -- The output stream to write output to
 * Return    : (CPU_State) The state of the new machine
 * Notes     : Keeps track of the input and output files
 */
CPU_State CPU_new(FILE *input, FILE *output) {
        CPU_State cpu = ALLOC(sizeof(*cpu));
        cpu -> mem = Mem_new();
        cpu -> input = input;
        cpu -> output = output;
        for (int i = 0; i < 8; i++) {
                cpu -> registers[i] = 0;
        }
        cpu -> programCounter = 0;
        cpu -> isRunning = true;
        cpu -> mainInstructionSize = 0;
        return cpu;
}

/*
 * Name      : CPU_free
 * Purpose   : Frees the CPU
 * Parameter : (CPU_State *) state -- The CPU_State to free
 * Return    : None
 * Notes     : Keeps track of the input and output files
 */
void CPU_free(CPU_State *state) {
        CPU_State cpu = *state;
        Mem mem = cpu -> mem;

        Mem_freeMemory(&mem);
        FREE(*state);
}

/*
 * Name      : initializeProgram
 * Purpose   : Loads the given instructions into the memory segment 0
 * Parameter : (CPU_State) state   -- The CPU to load the instructions into
 *             (FILE *)    program -- The program binary
 * Return    : None
 * Notes     : Will CRE if it runs out of memory;
 *             Will CRE if reading the file fails
 */

void initializeProgram(CPU_State state, FILE *program)
{       
        Seq_T instructions = Seq_new(1024);
        unsigned char byte = fgetc(program);
        while (!feof(program)) {
                uint32_t instruction = 0;
                for (int i = 3; i >= 0; i--) {
                        instruction = instruction | (byte << (i * 8));
                        // assert(!ferror(program));
                        byte = fgetc(program);
                }
                
                Seq_addhi(instructions, (void *)(uintptr_t)instruction);
        }
        
        Segment seg0 = Segment_new(Seq_length(instructions));
        state -> mainInstructionSize = Segment_size(seg0);
        int index = 0;
        while (Seq_length(instructions) > 0)
        {
                uint32_t instruction = 
                        (uint32_t)(uintptr_t) Seq_remlo(instructions);
                
                Segment_setWord(seg0, index, instruction);
                index++;
        }

        Mem mem = state -> mem;
        Mem_setSegment(mem, 0, seg0);

        Seq_free(&instructions);
}

/*
 * Name      : executeFunction
 * Purpose   : Fetches an instruction, then executes the instruction
 * Parameter : (CPU_State) state -- The current CPU state to modify/read/write
 * Return    : None
 * Notes     : Will CRE if it tried to read an instruction that was invalid
 */
void executeFunction(CPU_State state)
{
        uint32_t instruction = getInstruction(state);
        if (!(state -> isRunning)) {
                return;
        }
        

        uint32_t instructionType = instruction & instructionBits;

        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        switch(instructionType) {
                case CMOV:
                // fprintf(stderr, "CMOV\n");
                CPU_cmove(state, ra, rb, rc);
                break;

                case SEGLOAD:
                // fprintf(stderr, "SEGLOAD\n");
                CPU_segLoad(state, ra, rb, rc);
                break;

                case SEGSTORE:
                // fprintf(stderr, "SEGSTORE\n");
                CPU_segStore(state, ra, rb, rc);
                break;

                case ADD:
                // fprintf(stderr, "ADD r%u = r%u + r%u\n", ra, rb, rc);
                CPU_add(state, ra, rb, rc);
                break;

                case MUL:
                // fprintf(stderr, "MUL\n");
                CPU_mult(state, ra, rb, rc);
                break;

                case DIV:
                // fprintf(stderr, "DIV\n");
                CPU_div(state, ra, rb, rc);
                break;

                case NAND:
                // fprintf(stderr, "NAND\n");
                CPU_nand(state, ra, rb, rc);
                break;

                case HALT:
                // fprintf(stderr, "HALT\n");
                CPU_halt(state);
                break;

                case MAPSEG:
                // fprintf(stderr, "MAPSEG\n");
                CPU_mapSeg(state, rb, rc);
                break;

                case UNMAPSEG:
                // fprintf(stderr, "UNMAPSEG\n");
                CPU_unmapSeg(state, rc);
                break;

                case OUT:
                // fprintf(stderr, "OUT\n");
                CPU_printOut(state, rc);
                break;

                case IN:
                // fprintf(stderr, "IN\n");
                CPU_readIn(state, rc);
                break;

                case LOADPROGRAM:
                // fprintf(stderr, "LOADPROGRAM\n");
                CPU_loadProgram(state, rb, rc);
                break;

                case LOADVALUE:
                // fprintf(stderr, "LOADVALUE r%u = %u\n", (instruction & rLoadBits) >> rLoadLsb, instruction & loadBits);
                CPU_loadValue(state, instruction);
                break;

                default:
                CPU_halt(state);
                break;
        }
        
}

/*
 * Name      : getInstruction
 * Purpose   : Gets the instruction from the memory and increments the 
 *             memory counter
 * Parameter : (CPU_State) state -- The CPU state with the memory to read
 * Return    : (uint32_t) The 32-bit instruction
 * Notes     : Will return MAX value if the memory is out of bounds
 */
uint32_t getInstruction(CPU_State state)
{
        Mem mem = state -> mem;
        
        if (state -> programCounter >= state -> mainInstructionSize) {
                state -> isRunning = false; 
                return MAX_VALUE;
                
        }
        uint32_t instruction = Mem_getWord(mem, 0, state -> programCounter);
        state -> programCounter++;
        return instruction;
}

/*
 * Name      : CPU_cmove
 * Purpose   : Moves rb into ra if rc is not 0
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_cmove(CPU_State state, uint32_t ra, uint32_t rb, 
                             uint32_t rc)
{
        uint32_t *registers = state -> registers;
        if (registers[rc] == 0) {
                return;
        }
        registers[ra] = registers[rb];
}

/*
 * Name      : CPU_segLoad
 * Purpose   : Loads the word from memory in segment rb, word rc into ra
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_segLoad(CPU_State state, uint32_t ra, uint32_t rb, 
                               uint32_t rc)
{
        uint32_t *registers = state -> registers;
        Mem      mem        = state -> mem;
        registers[ra] = Mem_getWord(mem, registers[rb], registers[rc]);
}

/*
 * Name      : CPU_segStore
 * Purpose   : Stores rc into segment ra, word rb
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_segStore(CPU_State state, uint32_t ra, uint32_t rb, 
                                uint32_t rc)
{
        uint32_t *registers = state -> registers;
        Mem       mem       = state -> mem;
        Mem_setWord(mem, registers[ra], registers[rb], registers[rc]);
}

/*
 * Name      : CPU_add
 * Purpose   : Moves rb + rc into ra
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_add(CPU_State state, uint32_t ra, uint32_t rb, 
                           uint32_t rc)
{
        uint32_t *registers = state -> registers;
        registers[ra] = registers[rb] + registers[rc];
}

/*
 * Name      : CPU_mult
 * Purpose   : Moves rb * rc into ra
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_mult(CPU_State state, uint32_t ra, uint32_t rb, 
                            uint32_t rc)
{
        uint32_t *registers = state -> registers;
        registers[ra] = registers[rb] * registers[rc];
}

/*
 * Name      : CPU_div
 * Purpose   : Moves rb / rc into ra
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State;
 *             Will CRE if rc is 0
 */
 void CPU_div(CPU_State state, uint32_t ra, uint32_t rb, 
                           uint32_t rc)
{
        uint32_t *registers = state -> registers;
        registers[ra] = registers[rb] / registers[rc];
}

/*
 * Name      : CPU_nand
 * Purpose   : Moves rb nand rc into ra
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_nand(CPU_State state, uint32_t ra, uint32_t rb, 
                            uint32_t rc)
{
        uint32_t *registers = state -> registers;
        registers[ra] = ~(registers[rb] & registers[rc]);

}

/*
 * Name      : CPU_halt
 * Purpose   : Stops the program
 * Parameter : None
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_halt(CPU_State state)
{
        state -> isRunning = false;
}

/*
 * Name      : CPU_mapSeg
 * Purpose   : Makes a new segment with size rc, and stores the segmentID in rb
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_mapSeg(CPU_State state, uint32_t rb, uint32_t rc)
{
        uint32_t *registers = state -> registers;
        Mem      mem        = state -> mem;

        registers[rb] = Mem_mapNew(mem, registers[rc]);
}

/*
 * Name      : CPU_unmapSeg
 * Purpose   : Frees segment rc
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_unmapSeg(CPU_State state, uint32_t rc)
{
        uint32_t *registers = state -> registers;
        Mem      mem        = state -> mem;

        Mem_mapFree(mem, registers[rc]);
}

/*
 * Name      : CPU_printOut
 * Purpose   : Prints the character at rc
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Uses the output file from when CPU was initialized
 */
 void CPU_printOut(CPU_State state, uint32_t rc)
{
        uint32_t *registers = state -> registers;
        FILE     *output    = state -> output;

        char printChar = registers[rc];
        fprintf(output, "%c", printChar);
}

/*
 * Name      : CPU_readIn
 * Purpose   : Sets rc to the value of given input character
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State;
 *             Uses the input file from when CPU_State was initialized
 */
 void CPU_readIn(CPU_State state, uint32_t rc)
{
        uint32_t *registers = state -> registers;
        FILE     *input     = state -> input;

        
        int charVal = fgetc(input);
        if (charVal != -1) {
                registers[rc] = charVal;
        }
        else {
                registers[rc] = MAX_VALUE;
        }
        
}

/*
 * Name      : CPU_loadProgram
 * Purpose   : Loads a copy of segment rb into segment 0, then sets the 
 *             program counter to rc
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
 void CPU_loadProgram(CPU_State state, uint32_t rb, uint32_t rc)
{
        uint32_t *registers = state -> registers;
        Mem      mem        = state -> mem;

        state -> programCounter = registers[rc];

        if (registers[rb] == 0) {
                return;
        }

        Segment seg  = Mem_getSegment(mem, registers[rb]);
        Segment copy = Segment_copy(seg);
        state -> mainInstructionSize = Segment_size(copy);
        // fprintf(stderr, "New Segment 0 Size: %i\n", Segment_size(copy));
        Mem_setSegment(mem, 0, copy);
}

/*
 * Name      : CPU_loadValue
 * Purpose   : Loads a given value into rL 
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 * Return    : None
 * Notes     : Alters the state of the CPU_State;
 */
 void CPU_loadValue(CPU_State state, uint32_t instruction)
{
        uint32_t rL = (instruction & rLoadBits) >> rLoadLsb;
        uint32_t loadVal = instruction & loadBits;
        uint32_t *registers = state -> registers;
        registers[rL] = loadVal;
}

#undef MAX_VALUE 

#undef loadBits 
#undef instruction
#undef raBits 
#undef rbBits 
#undef rcBits 
#undef rLoadBits 
#undef raLsb 
#undef rbLsb 
#undef rLoadLsb 

#undef CMOV         
#undef SEGLOAD      
#undef SEGSTORE     
#undef ADD          
#undef MUL          
#undef DIV          
#undef NAND         
#undef HALT         
#undef MAPSEG       
#undef UNMAPSEG     
#undef OUT          
#undef IN           
#undef LOADPROGRAM  
#undef LOADVALUE    