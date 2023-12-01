/* 
 *   Name       : CPU.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module responsible for performs the arithmetic and
 *                logical operations of the Universal Machine; fetches,
 *                decodes, and executes instructions from cpu.memory
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
 * Purpose : Represents the current  of the computer at any given time
 * Notes   : Only used inside of CPU
 */
// typedef struct CPU_State {
//         Mem cpu.mem;
//         FILE *cpu.input;
//         FILE *cpu.output;
//         uint32_t cpu.registers[8];
//         uint32_t cpu.programCounter;
//         bool cpu.isRunning;
//         uint32_t cpu.mainInstructionSize;
// } *CPU_State;


typedef struct CPU_State {
        Mem mem;
        FILE *input;
        FILE *output;
        uint32_t registers[8];
        uint32_t programCounter;
        uint32_t mainInstructionSize;
        bool isRunning;
        
} CPU_State;



void runProgram(FILE *inFile, FILE *outFile, FILE *program);

// CPU_State CPU_new(FILE *cpu.input, FILE *cpu.output);
// void CPU_free(CPU_State *);
void initializeProgram(FILE *program);

uint32_t getInstruction();
void executeFunction();

void CPU_cmove(uint32_t ra, uint32_t rb, 
                            uint32_t rc);
void CPU_segLoad(uint32_t ra, uint32_t rb, 
                              uint32_t rc);
void CPU_segStore(uint32_t ra, uint32_t rb, 
                               uint32_t rc);
void CPU_add(uint32_t ra, uint32_t rb, 
                          uint32_t rc);
void CPU_mult(uint32_t ra, uint32_t rb, 
                           uint32_t rc);
void CPU_div(uint32_t ra, uint32_t rb, 
                          uint32_t rc);
void CPU_nand(uint32_t ra, uint32_t rb, 
                           uint32_t rc);
void CPU_halt();
void CPU_mapSeg(uint32_t rb, uint32_t rc);
void CPU_unmapSeg(uint32_t rc);
void CPU_printOut(uint32_t rc);
void CPU_readIn(uint32_t rc);
void CPU_loadProgram(uint32_t rb, uint32_t rc);
void CPU_loadValue(uint32_t instruction);

CPU_State cpu = {
        .mem                 = NULL,
        .input               = NULL,
        .output              = NULL,
        .registers           = {0, 0, 0, 0, 0, 0, 0, 0},
        .programCounter      = 0,
        .mainInstructionSize = 0,
        .isRunning           = true
};
/*
 * Name      : runProgram
 * Purpose   : Emulates the Universal Machine with the given program and 
 *             the given cpu.input, cpu.output streams
 * Parameter : (FILE *) cpu.input   -- The cpu.input stream to read cpu.input from
 *             (FILE *) cpu.output  -- The cpu.output stream to write cpu.output to
 *             (FILE *) program -- The .um binary file with the program
 * Return    : None
 * Notes     : Runs the entire program, and only ends when it reaches the end
 *             of instructions, or the program halts
 */
void runProgram(FILE *inFile, FILE *outFile, FILE *program)
{
        cpu.mem                 = Mem_new();
        cpu.input               = inFile;
        cpu.output              = outFile;
        cpu.programCounter      = 0;
        cpu.mainInstructionSize = 0;
        cpu.isRunning           = true;
        for (int i = 0; i < 8; i++) {
                cpu.registers[i] = 0;
        }
        
        initializeProgram(program);
        while (cpu.isRunning) {
                executeFunction();
        }
        
        Mem_freeMemory(&cpu.mem);
}

/*
 * Name      : CPU_new
 * Purpose   : Creates the CPU  that represents a new CPU
 * Parameter : (FILE *) cpu.input   -- The cpu.input stream to read cpu.input from
 *             (FILE *) cpu.output  -- The cpu.output stream to write cpu.output to
 * Return    : (CPU_State) The  of the new machine
 * Notes     : Keeps track of the cpu.input and cpu.output files
 */
// CPU_State CPU_new(FILE *cpu.input, FILE *cpu.output) {
//         CPU_State cpu = ALLOC(sizeof(*cpu));
//         cpu -> cpu.mem = Mem_new();
//         cpu -> cpu.input = cpu.input;
//         cpu -> cpu.output = cpu.output;
//         for (int i = 0; i < 8; i++) {
//                 cpu -> cpu.registers[i] = 0;
//         }
//         cpu -> cpu.programCounter = 0;
//         cpu -> cpu.isRunning = true;
//         cpu -> cpu.mainInstructionSize = 0;
//         return cpu;
// }

/*
 * Name      : CPU_free
 * Purpose   : Frees the CPU
 * Parameter : (CPU_State *)  -- The CPU_State to free
 * Return    : None
 * Notes     : Keeps track of the cpu.input and cpu.output files
 */
// void CPU_free(CPU_State *) {
//         CPU_State cpu = *;
//         Mem cpu.mem = cpu -> cpu.mem;

//         Mem_freeMemory(&cpu.mem);
//         FREE(*);
// }

/*
 * Name      : initializeProgram
 * Purpose   : Loads the given instructions into the cpu.memory segment 0
 * Parameter : (CPU_State)    -- The CPU to load the instructions into
 *             (FILE *)    program -- The program binary
 * Return    : None
 * Notes     : Will CRE if it runs out of cpu.memory;
 *             Will CRE if reading the file fails
 */

void initializeProgram(FILE *program)
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
        cpu.mainInstructionSize = Segment_size(seg0);
        int index = 0;
        while (Seq_length(instructions) > 0)
        {
                uint32_t instruction = 
                        (uint32_t)(uintptr_t) Seq_remlo(instructions);
                
                Segment_setWord(seg0, index, instruction);
                index++;
        }

        Mem_setSegment(cpu.mem, 0, seg0);

        Seq_free(&instructions);
}

/*
 * Name      : executeFunction
 * Purpose   : Fetches an instruction, then executes the instruction
 * Parameter : (CPU_State)  -- The current CPU  to modify/read/write
 * Return    : None
 * Notes     : Will CRE if it tried to read an instruction that was invalid
 */
void executeFunction()
{
        uint32_t instruction = getInstruction();
        if (!(cpu.isRunning)) {
                return;
        }
        

        uint32_t instructionType = instruction & instructionBits;

        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        switch(instructionType) {
                case CMOV:
                // fprintf(stderr, "CMOV\n");
                CPU_cmove(ra, rb, rc);
                break;

                case SEGLOAD:
                // fprintf(stderr, "SEGLOAD\n");
                CPU_segLoad(ra, rb, rc);
                break;

                case SEGSTORE:
                // fprintf(stderr, "SEGSTORE\n");
                CPU_segStore(ra, rb, rc);
                break;

                case ADD:
                // fprintf(stderr, "ADD r%u = r%u + r%u\n", ra, rb, rc);
                CPU_add(ra, rb, rc);
                break;

                case MUL:
                // fprintf(stderr, "MUL\n");
                CPU_mult(ra, rb, rc);
                break;

                case DIV:
                // fprintf(stderr, "DIV\n");
                CPU_div(ra, rb, rc);
                break;

                case NAND:
                // fprintf(stderr, "NAND\n");
                CPU_nand(ra, rb, rc);
                break;

                case HALT:
                // fprintf(stderr, "HALT\n");
                CPU_halt();
                break;

                case MAPSEG:
                // fprintf(stderr, "MAPSEG\n");
                CPU_mapSeg(rb, rc);
                break;

                case UNMAPSEG:
                // fprintf(stderr, "UNMAPSEG\n");
                CPU_unmapSeg(rc);
                break;

                case OUT:
                // fprintf(stderr, "OUT\n");
                CPU_printOut(rc);
                break;

                case IN:
                // fprintf(stderr, "IN\n");
                CPU_readIn(rc);
                break;

                case LOADPROGRAM:
                // fprintf(stderr, "LOADPROGRAM\n");
                CPU_loadProgram(rb, rc);
                break;

                case LOADVALUE:
                // fprintf(stderr, "LOADVALUE r%u = %u\n", (instruction & rLoadBits) >> rLoadLsb, instruction & loadBits);
                CPU_loadValue(instruction);
                break;

                default:
                CPU_halt();
                break;
        }
        // // Register Reader
        // for (int i = 0; i < 8; i++) {
        //         fprintf(stderr, "Register r%i := %i\n", i, cpu.registers[i]);
        // }
}

/*
 * Name      : getInstruction
 * Purpose   : Gets the instruction from the cpu.memory and increments the 
 *             cpu.memory counter
 * Parameter : (CPU_State)  -- The CPU  with the cpu.memory to read
 * Return    : (uint32_t) The 32-bit instruction
 * Notes     : Will return MAX value if the cpu.memory is out of bounds
 */
uint32_t getInstruction()
{
        if (cpu.programCounter >= cpu.mainInstructionSize) {
                cpu.isRunning = false; 
                return MAX_VALUE;
                
        }
        uint32_t instruction = Mem_getWord(cpu.mem, 0, cpu.programCounter);
        cpu.programCounter++;
        return instruction;
}

/*
 * Name      : CPU_cmove
 * Purpose   : Moves rb into ra if rc is not 0
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_cmove(uint32_t ra, uint32_t rb, 
                             uint32_t rc)
{
        if (cpu.registers[rc] == 0) {
                return;
        }
        cpu.registers[ra] = cpu.registers[rb];
}

/*
 * Name      : CPU_segLoad
 * Purpose   : Loads the word from cpu.memory in segment rb, word rc into ra
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_segLoad(uint32_t ra, uint32_t rb, 
                               uint32_t rc)
{
        cpu.registers[ra] = Mem_getWord(cpu.mem, cpu.registers[rb], cpu.registers[rc]);
}

/*
 * Name      : CPU_segStore
 * Purpose   : Stores rc into segment ra, word rb
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_segStore(uint32_t ra, uint32_t rb, 
                                uint32_t rc)
{
        Mem_setWord(cpu.mem, cpu.registers[ra], cpu.registers[rb], cpu.registers[rc]);
}

/*
 * Name      : CPU_add
 * Purpose   : Moves rb + rc into ra
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_add(uint32_t ra, uint32_t rb, 
                           uint32_t rc)
{
        cpu.registers[ra] = cpu.registers[rb] + cpu.registers[rc];
}

/*
 * Name      : CPU_mult
 * Purpose   : Moves rb * rc into ra
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_mult(uint32_t ra, uint32_t rb, 
                            uint32_t rc)
{
        cpu.registers[ra] = cpu.registers[rb] * cpu.registers[rc];
}

/*
 * Name      : CPU_div
 * Purpose   : Moves rb / rc into ra
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State;
 *             Will CRE if rc is 0
 */
 void CPU_div(uint32_t ra, uint32_t rb, 
                           uint32_t rc)
{
        cpu.registers[ra] = cpu.registers[rb] / cpu.registers[rc];
}

/*
 * Name      : CPU_nand
 * Purpose   : Moves rb nand rc into ra
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_nand(uint32_t ra, uint32_t rb, 
                            uint32_t rc)
{
        cpu.registers[ra] = ~(cpu.registers[rb] & cpu.registers[rc]);

}

/*
 * Name      : CPU_halt
 * Purpose   : Stops the program
 * Parameter : None
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_halt()
{
        cpu.isRunning = false;
}

/*
 * Name      : CPU_mapSeg
 * Purpose   : Makes a new segment with size rc, and stores the segmentID in rb
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_mapSeg(uint32_t rb, uint32_t rc)
{
        cpu.registers[rb] = Mem_mapNew(cpu.mem, cpu.registers[rc]);
}

/*
 * Name      : CPU_unmapSeg
 * Purpose   : Frees segment rc
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_unmapSeg(uint32_t rc)
{
        Mem_mapFree(cpu.mem, cpu.registers[rc]);
}

/*
 * Name      : CPU_printOut
 * Purpose   : Prints the character at rc
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Uses the cpu.output file from when CPU was initialized
 */
 void CPU_printOut(uint32_t rc)
{
        char printChar = cpu.registers[rc];
        fprintf(cpu.output, "%c", printChar);
}

/*
 * Name      : CPU_readIn
 * Purpose   : Sets rc to the value of given cpu.input character
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State;
 *             Uses the cpu.input file from when CPU_State was initialized
 */
 void CPU_readIn(uint32_t rc)
{
        int charVal = fgetc(cpu.input);
        if (charVal != -1) {
                cpu.registers[rc] = charVal;
        }
        else {
                cpu.registers[rc] = MAX_VALUE;
        }
        
}

/*
 * Name      : CPU_loadProgram
 * Purpose   : Loads a copy of segment rb into segment 0, then sets the 
 *             program counter to rc
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the  of the CPU_State
 */
 void CPU_loadProgram(uint32_t rb, uint32_t rc)
{
        cpu.programCounter = cpu.registers[rc];

        if (cpu.registers[rb] == 0) {
                return;
        }

        Segment seg  = Mem_getSegment(cpu.mem, cpu.registers[rb]);
        Segment copy = Segment_copy(seg);
        cpu.mainInstructionSize = Segment_size(copy);
        Mem_setSegment(cpu.mem, 0, copy);
}

/*
 * Name      : CPU_loadValue
 * Purpose   : Loads a given value into rL 
 * Parameter : (CPU_State)  -- The computer  to alter (with register)
 * Return    : None
 * Notes     : Alters the  of the CPU_State;
 */
 void CPU_loadValue(uint32_t instruction)
{
        uint32_t rL = (instruction & rLoadBits) >> rLoadLsb;
        uint32_t loadVal = instruction & loadBits;
        cpu.registers[rL] = loadVal;
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