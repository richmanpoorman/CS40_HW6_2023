/* 
 *   Name       : um.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Driver file for our UM implementation
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

// #include "CPU.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* 
 *   Name       : CPU.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module responsible for performs the arithmetic and
 *                logical operations of the Universal Machine; fetches,
 *                decodes, and executes instructions from memory
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

// #include "Memory.h"
#include <stdint.h>
#include <mem.h>
#include <stdbool.h>
#include <stdio.h>
// #include <seq.h>
// #include "Segment.h"
// #include <assert.h>

#define MAX_VALUE 0xffffffff

#define numInitialInstructions 256

#define loadBits 0x01ffffff
#define instructionBits 0xf0000000
#define instructionLSB 28
#define raBits 0x000001c0
#define rbBits 0x00000038
#define rcBits 0x00000007
#define rLoadBits 0x0e000000
#define raLsb 6
#define rbLsb 3
#define rLoadLsb 25
typedef enum Instruction {
        CMOV         = 0,
        SEGLOAD      = 1,
        SEGSTORE     = 2,
        ADD          = 3,
        MUL          = 4,
        DIV          = 5,
        NAND         = 6,
        HALT         = 7,
        MAPSEG       = 8,
        UNMAPSEG     = 9,
        OUT          = 10,
        IN           = 11,
        LOADPROGRAM  = 12,
        LOADVALUE    = 13
} Instruction;

/*
 * Name    : CPU_State
 * Purpose : Represents the current state of the computer at any given time
 * Notes   : Only used inside of CPU
 */



#define START_SIZE 256

typedef uint32_t Word;
typedef struct Segment {
        Word *words;
        uint32_t size;
} Segment;


typedef uint32_t SegmentID;
typedef struct Mem {
        Segment  *segments;
        uint32_t  segmentsSize;
        uint32_t  segmentsCapacity;

        uint32_t *nextSeenSegID;
        uint32_t  nextSeenCapacity;
        uint32_t  nextSeenSize;
} *Mem;



typedef struct CPU_State {
        FILE *input;
        FILE *output;
        uint32_t registers[8];
        uint32_t programCounter;
        bool isRunning;
        uint32_t mainInstructionSize;
} *CPU_State;


static inline void runProgram(FILE *input, FILE *output, FILE *program);

static inline CPU_State CPU_new(FILE *input, FILE *output);
// void initializeProgram(CPU_State state, Mem mem, FILE *program);
// void executeFunction(CPU_State state, Mem mem);

static inline void CPU_cmove(uint32_t *registers, uint32_t instruction);
//  void CPU_segLoad(uint32_t *registers,Mem mem, uint32_t instruction);
//  void CPU_segStore(uint32_t *registers,Mem mem, uint32_t instruction);
static inline void CPU_add(uint32_t *registers, uint32_t instruction);
static inline void CPU_mult(uint32_t *registers ,uint32_t instruction);
static inline void CPU_div(uint32_t *registers, uint32_t instruction);
static inline void CPU_nand(uint32_t *registers, uint32_t instruction);
static inline void CPU_mapSeg(uint32_t *registers, Mem mem, uint32_t instruction);
static inline void CPU_unmapSeg(uint32_t *registers, Mem mem, uint32_t instruction);
static inline void CPU_printOut(uint32_t *registers, FILE *output, uint32_t instruction);
static inline void CPU_readIn(uint32_t *registers, FILE *input, uint32_t instruction);
//  void CPU_loadProgram(uint32_t *registers,Mem mem, uint32_t instruction);
static inline void CPU_loadValue(uint32_t *registers,uint32_t instruction);


static inline Mem Mem_new();
static inline void Mem_freeMemory(Mem *mem);
static inline SegmentID Mem_mapNew(Mem mem, uint32_t seg);
static inline void Mem_mapFree(Mem mem, uint32_t segID);

static inline void expandSegments(Mem mem);
static inline void expandNextSeen(Mem mem);

static inline void expandSegments(Mem mem)
{
        
        uint32_t size        = mem -> segmentsCapacity;
        uint32_t newSize     = 2 * size;
        Segment *newSegments = CALLOC(newSize, sizeof(Segment));
        Segment *segments    = mem -> segments;
        for (uint32_t i = 0; i < size; i++) {
                newSegments[i].words = segments[i].words;
                newSegments[i].size  = segments[i].size;
        }
        FREE(mem -> segments);
        mem -> segments = newSegments;
        mem -> segmentsCapacity = newSize;
}
static inline void expandNextSeen(Mem mem)
{
        
        uint32_t  size        = mem -> nextSeenCapacity;
        uint32_t  newSize     = 2 * size;
        uint32_t *newNextSeen = CALLOC(newSize, sizeof(uint32_t));
        uint32_t *nextSeen    = mem -> nextSeenSegID;

        for (uint32_t i = 0; i < size; i++) {
                newNextSeen[i] = nextSeen[i];
        }
        FREE(mem -> nextSeenSegID);
        mem -> nextSeenSegID = newNextSeen;
        mem -> nextSeenCapacity  = newSize;
}


/*
 *  Name      : Mem_new
 *  Purpose   : Creates a new memory representation
 *  Parameters: (None)
 *  Returns   : (None)
 *  Notes     : Will CRE on failure to allocate memory
 *              
 */
static inline Mem Mem_new() 
{
        Mem mem  = ALLOC(sizeof(*mem));
        mem -> segments     = CALLOC(START_SIZE, sizeof(Segment));
        mem -> segments[0].size = 0;
        mem -> segments[0].words = NULL;
        mem -> segmentsCapacity = START_SIZE;
        mem -> segmentsSize = 1;

        mem -> nextSeenSegID = CALLOC(START_SIZE, sizeof(uint32_t));
        mem -> nextSeenCapacity  = START_SIZE;
        mem -> nextSeenSize = 0;

        return mem;
}

/*
 * Name      : Mem_freeMemory
 * Purpose   : Frees the memory representation
 * Parameter : (Mem *) mem -- The pointer to the Mem to free
 * Return    : None
 * Notes     : Will CRE if mem is null or *mem is null;
 *             Will set the value in mem to NULL
 */
static inline void Mem_freeMemory(Mem *mem)
{
        Mem memory = *mem;
        uint32_t size = memory -> segmentsSize;
        Segment *segments = memory -> segments;
        for (uint32_t i = 0; i < size; i++) {
                if (segments[i].words == NULL) {
                        continue;
                } 
                FREE(segments[i].words);
        }

        FREE(memory -> segments);
        FREE(memory -> nextSeenSegID);
        FREE(*mem); 
}

/*
 * Name      : Mem_mapNew
 * Purpose   : Creates a new segment in memory
 * Parameter : (Mem) mem -- The memory block to add the segment to
 *             (uint32_t) size -- Size of the segment to add
 * Return    : None
 * Notes     : Will CRE if mem is NULL
 */
static inline SegmentID Mem_mapNew(Mem mem, uint32_t size) 
{
        // fprintf(stderr, "Map segment\n");
        
        if (mem -> nextSeenSize == 0) {
                if (mem -> segmentsCapacity == mem -> segmentsSize) {
                        expandSegments(mem);
                }

                if (size != 0)
                        mem -> segments[mem -> segmentsSize].words = CALLOC(size, sizeof(uint32_t));
                else 
                        mem -> segments[mem -> segmentsSize].words = NULL;

                mem -> segments[mem -> segmentsSize].size  = size;

                uint32_t index = mem -> segmentsSize;
                mem -> segmentsSize++;
                // fprintf(stderr, "End Map segment (unseen)\n");
                return index;
        }
        mem -> nextSeenSize--;
        uint32_t nextPosition = mem -> nextSeenSize; 
        uint32_t index = mem -> nextSeenSegID[nextPosition];
        if (size != 0)
                mem -> segments[index].words = CALLOC(size, sizeof(uint32_t));
        else 
                mem -> segments[mem -> segmentsSize].words = NULL;
        mem -> segments[index].size = size;
        
        return index;

}

/*
 * Name      : Mem_mapFree
 * Purpose   : Frees a segment in memory
 * Parameter : (Mem) mem -- The memory block with the segment to free
 *             (Segment) seg -- The 32-bit memory identifier of the segment
 * Return    : None
 * Notes     : Will CRE if mem is NULL or the segID is greater than the 
 *             memory size
 */
static inline void Mem_mapFree(Mem mem, uint32_t segID) 
{
        
        FREE(mem -> segments[segID].words);
        uint32_t nextPos = mem -> nextSeenSize;
        if (mem -> nextSeenCapacity == nextPos) {
                expandNextSeen(mem);
        }
        mem -> nextSeenSegID[nextPos] = segID;
        mem -> nextSeenSize++;
}


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
static inline void runProgram(FILE *input, FILE *output, FILE *program)
{
        // CPU STATE
        uint32_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint32_t programCounter = 0;
        uint32_t mainInstructionSize = 0;
        Mem mem = Mem_new();

        // INITIALIZE PROGRAM //
        uint32_t  capacity     = numInitialInstructions;
        uint32_t *instructions = CALLOC(capacity, sizeof(uint32_t));
        uint32_t  index        = 0;
        
        unsigned char byte = fgetc(program);
        while (!feof(program)) {
                uint32_t instruction = 0;
                for (int i = 3; i >= 0; i--) {
                        instruction = instruction | (byte << (i * 8));
                        // assert(!ferror(program));
                        byte = fgetc(program);
                }
                instructions[index] = instruction;
                index++;

                // Expansion //
                if (index == capacity) {
                        capacity *= 2;
                        uint32_t *newInstructions = CALLOC(capacity, sizeof(uint32_t));
                        for (uint32_t i = 0; i < index; i++) {
                                newInstructions[i] = instructions[i];
                        }
                        FREE(instructions);
                        instructions = newInstructions;
                }
                // Seq_addhi(instructions, (void *)(uintptr_t)instruction);
        }
        
        // uint32_t numInstructions = Seq_length(instructions);
        mem -> segments[0].words = instructions;
        mem -> segments[0].size  = index;
        
        mainInstructionSize = index;
        // END INITIALIZE PROGRAM // 


        // initializeProgram(state, mem, program);
        
        uint32_t *mainProgram = mem -> segments[0].words;
        // RUN PROGRAM // 
        while (programCounter < mainInstructionSize) {
                uint32_t instruction = mainProgram[programCounter]; // Mem_getWord(mem, 0, programCounter);
                programCounter++;

                // END OF GET INSTRUCTION
                Instruction instructionType = (instruction & instructionBits) >> instructionLSB;

                switch(instructionType) {
                        case CMOV:
                        // fprintf(stderr, "CMOV\n");
                        CPU_cmove(registers, instruction);
                        break;

                        case SEGLOAD:
                        // fprintf(stderr, "SEGLOAD\n");
                        registers[(instruction & raBits) >> raLsb] = mem -> segments[registers[(instruction & rbBits) >> rbLsb]].words[registers[instruction & rcBits]]; // Mem_getWord(mem, registers[rb], registers[rc]);
                        break;

                        case SEGSTORE:
                        // fprintf(stderr, "SEGSTORE\n");
                        mem -> segments[registers[(instruction & raBits) >> raLsb]].words[registers[(instruction & rbBits) >> rbLsb]] = registers[instruction & rcBits];
                        break;

                        case ADD:
                        // fprintf(stderr, "ADD r%u = r%u + r%u\n", ra, rb, rc);
                        CPU_add(registers, instruction);
                        break;

                        case MUL:
                        // fprintf(stderr, "MUL\n");
                        CPU_mult(registers, instruction);
                        break;

                        case DIV:
                        // fprintf(stderr, "DIV\n");
                        CPU_div(registers, instruction);
                        break;

                        case NAND:
                        // fprintf(stderr, "NAND\n");
                        CPU_nand(registers, instruction);
                        break;

                        // case HALT:
                        
                        // programCounter = mainInstructionSize;
                        // break;

                        case MAPSEG:
                        // fprintf(stderr, "MAPSEG\n");
                        CPU_mapSeg(registers, mem, instruction);
                        break;

                        case UNMAPSEG:
                        // fprintf(stderr, "UNMAPSEG\n");
                        CPU_unmapSeg(registers, mem, instruction);
                        break;

                        case OUT:
                        // fprintf(stderr, "OUT\n");
                        CPU_printOut(registers, output, instruction);
                        break;

                        case IN:
                        // fprintf(stderr, "IN\n");
                        CPU_readIn(registers, input, instruction);
                        break;

                        case LOADPROGRAM:
                        // fprintf(stderr, "LOADPROGRAM\n");
                        {
                                uint32_t rb = (instruction & rbBits) >> rbLsb;
                                programCounter = registers[instruction & rcBits];
                                if (registers[rb] != 0) {
                                        Segment seg = mem -> segments[registers[rb]];
                                        uint32_t programSegSize = seg.size;
                                        uint32_t *programSegWords = seg.words;
                                        if (programSegSize > mainInstructionSize) {
                                                FREE(mainProgram);
                                                mainProgram = CALLOC(programSegSize, sizeof(uint32_t));
                                                mem -> segments[0].words = mainProgram;
                                        }
                                        

                                        for (uint32_t i = 0; i < programSegSize; i++) {
                                                mainProgram[i] = programSegWords[i];
                                        }
                                        mainInstructionSize = programSegSize;
                                        // FREE(mainProgram); // By case above, r[rb] != 0
                                        
                                        // mainProgram = CALLOC(seg.size, sizeof(uint32_t));
                                        // mem -> segments[0].size  = seg.size;
                                        // mem -> segments[0].words = mainProgram;
                                        // for (uint32_t i = 0; i < seg.size; i++) {
                                        //         mainProgram[i] = seg.words[i];
                                        // }
                                        // mainInstructionSize = seg.size;
                                        
                                }
                        }
                        break;

                        case LOADVALUE:
                        // fprintf(stderr, "LOADVALUE r%u = %u\n", (instruction & rLoadBits) >> rLoadLsb, instruction & loadBits);
                        CPU_loadValue(registers, instruction);
                        break;

                        default:
                        
                        programCounter = mainInstructionSize;
                        break;
                }
        }
        
        // END RUN PROGRAM //
        Mem_freeMemory(&mem);
}

/*
 * Name      : CPU_new
 * Purpose   : Creates the CPU state that represents a new CPU
 * Parameter : (FILE *) input   -- The input stream to read input from
 *             (FILE *) output  -- The output stream to write output to
 * Return    : (CPU_State) The state of the new machine
 * Notes     : Keeps track of the input and output files
 */
static inline CPU_State CPU_new(FILE *input, FILE *output) {
        CPU_State cpu = ALLOC(sizeof(*cpu));
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
 * Name      : CPU_cmove
 * Purpose   : Moves rb into ra if rc is not 0
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 *             (uint32_t)  ra    -- Register A
 *             (uint32_t)  rb    -- Register B
 *             (uint32_t)  rc    -- Register C
 * Return    : None
 * Notes     : Alters the state of the CPU_State
 */
static inline void CPU_cmove(uint32_t *registers,uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        
        if (registers[rc] == 0) {
                return;
        }
        registers[ra] = registers[rb];
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
static inline void CPU_add(uint32_t *registers,uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        
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
static inline void CPU_mult(uint32_t *registers,uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        
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
static inline void CPU_div(uint32_t *registers,uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        
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
static inline void CPU_nand(uint32_t *registers,uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        
        registers[ra] = ~(registers[rb] & registers[rc]);

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
static inline void CPU_mapSeg(uint32_t *registers,Mem mem, uint32_t instruction)
{
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        

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
static inline void CPU_unmapSeg(uint32_t *registers,Mem mem, uint32_t instruction)
{
        uint32_t rc = instruction & rcBits;

        

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
static inline void CPU_printOut(uint32_t *registers, FILE *output, uint32_t instruction)
{
        uint32_t rc = instruction & rcBits;

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
static inline void CPU_readIn(uint32_t *registers, FILE *input, uint32_t instruction)
{
        uint32_t rc = instruction & rcBits;

        registers[rc] = fgetc(input);
        
}


/*
 * Name      : CPU_loadValue
 * Purpose   : Loads a given value into rL 
 * Parameter : (CPU_State) state -- The computer state to alter (with register)
 * Return    : None
 * Notes     : Alters the state of the CPU_State;
 */
static inline void CPU_loadValue(uint32_t *registers,uint32_t instruction)
{
        uint32_t rL = (instruction & rLoadBits) >> rLoadLsb;
        uint32_t loadVal = instruction & loadBits;
        
        registers[rL] = loadVal;
}



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


#undef MAX_VALUE 

#undef numInitialInstructions

#undef loadBits 
#undef instruction
#undef raBits 
#undef rbBits 
#undef rcBits 
#undef rLoadBits 
#undef raLsb 
#undef rbLsb 
#undef rLoadLsb 
