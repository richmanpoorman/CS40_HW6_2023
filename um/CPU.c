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
#include <seq.h>
// #include "Segment.h"
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



#define START_SIZE 256

typedef uint32_t Word;
typedef struct Segment {
        Word *words;
        uint32_t size;
} Segment;

// Segment Segment_new();
// Segment Segment_copy(Segment seg);
// void Segment_free(Segment *seg);
// uint32_t Segment_size(Segment seg);
// Word Segment_getWord(Segment seg, uint32_t position);
// void Segment_setWord(Segment seg, uint32_t position, Word word);

typedef uint32_t SegmentID;
typedef struct Mem {
        // Seq_T segments;
        // Seq_T nextSeenSegID;
        Segment  *segments;
        uint32_t  segmentsSize;
        uint32_t  segmentsCapacity;

        uint32_t *nextSeenSegID;
        uint32_t  nextSeenCapacity;
        uint32_t  nextSeenSize;
} *Mem;



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
void executeFunction(CPU_State state);

 void CPU_cmove(CPU_State state, uint32_t instruction);
 void CPU_segLoad(CPU_State state, uint32_t instruction);
 void CPU_segStore(CPU_State state, uint32_t instruction);
 void CPU_add(CPU_State state, uint32_t instruction);
 void CPU_mult(CPU_State state, uint32_t instruction);
 void CPU_div(CPU_State state, uint32_t instruction);
 void CPU_nand(CPU_State state, uint32_t instruction);
 void CPU_mapSeg(CPU_State state, uint32_t instruction);
 void CPU_unmapSeg(CPU_State state, uint32_t instruction);
 void CPU_printOut(CPU_State state, uint32_t instruction);
 void CPU_readIn(CPU_State state, uint32_t instruction);
 void CPU_loadProgram(CPU_State state, uint32_t instruction);
 void CPU_loadValue(CPU_State state, uint32_t instruction);


Mem Mem_new();
void Mem_freeMemory(Mem *mem);
// Segment Mem_getSegment(Mem mem, uint32_t segID);
// void Mem_setSegment(Mem mem, uint32_t segID, Segment seg);
// Word Mem_getWord(Mem mem, uint32_t segID, uint32_t wordID);
// void Mem_setWord(Mem mem, uint32_t segID, uint32_t wordID, 
//                     uint32_t value);
SegmentID Mem_mapNew(Mem mem, uint32_t seg);
void Mem_mapFree(Mem mem, uint32_t segID);

void expandSegments(Mem mem);
void expandNextSeen(Mem mem);

void expandSegments(Mem mem)
{
        
        uint32_t size        = mem -> segmentsCapacity;
        uint32_t newSize     = 2 * size;
        Segment *newSegments = CALLOC(newSize, sizeof(Segment));
        Segment *segments    = mem -> segments;
        for (uint32_t i = 0; i < size; i++) {
                newSegments[i].words = segments[i].words,
                newSegments[i].size  = segments[i].size;
                
        }
        FREE(mem -> segments);
        mem -> segments = newSegments;
        mem -> segmentsCapacity = newSize;
}
void expandNextSeen(Mem mem)
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
Mem Mem_new() 
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
void Mem_freeMemory(Mem *mem)
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

// /*
//  * Name      : Mem_getSegment
//  * Purpose   : Gets the full segment at a given memory id
//  * Parameter : (Mem) mem -- The memory block containing the segment
//  *             (uint32_t) segID -- The 32-bit memory identifier of the segment
//  * Return    : None
//  * Notes     : Will CRE if mem is NULL or the segID is greater than the 
//  *             memory size
//  */
// Segment Mem_getSegment(Mem mem, uint32_t segID) 
// {
        
//         return mem -> segments[segID];
// }

// /*
//  * Name      : Mem_setSegment
//  * Purpose   : Sets the segment in memory to a given value
//  * Parameter : (Mem) mem -- The memory block containing the segment
//  *             (uint32_t) segID -- The 32-bit memory identifier of the segment
//  *             (Segment) seg -- The value to set the segment to
//  * Return    : None
//  * Notes     : Will CRE if mem is NULL 
//  */
// void Mem_setSegment(Mem mem, uint32_t segID, Segment seg) 
// {
//         FREE(mem -> segments[segID].words);
//         mem -> segments[segID] = {
//                 seg.words,
//                 seg.size
//         };
        
// }

/*
 * Name      : Mem_getWord
 * Purpose   : Gets the word at a given position in a given memory segment
 * Parameter : (Mem) mem -- The memory block containing the segment
 *             (uint32_t) segID -- The memory identifier of the segment
 *             (uint32_t) wordID -- The word identifier within the segment
 * Return    : None
 * Notes     : Will CRE if mem is NULL or the segID is greater than the 
 *             memory size
 */
// Word Mem_getWord(Mem mem, uint32_t segID, uint32_t wordID) 
// {
        
//         return mem -> segments[segID].words[wordID];
// }

// /*
//  * Name      : Mem_setWord
//  * Purpose   : Sets the word at a given position in a given memory segment
//  * Parameter : (Mem) mem -- The memory block containing the segment
//  *             (uint32_t) segID -- The memory identifier of the segment
//  *             (uint32_t) wordID -- The word identifier within the segment
//  *             (uint32_t) value -- The value to set the word to
//  * Return    : None
//  * Notes     : Will CRE if mem is NULL or the segID is greater than the 
//  *             memory size
//  */
// void Mem_setWord(Mem mem, uint32_t segID, uint32_t wordID, 
//                     uint32_t value) 
// {
//         mem -> segments[segID].words[wordID] = value;
// }

/*
 * Name      : Mem_mapNew
 * Purpose   : Creates a new segment in memory
 * Parameter : (Mem) mem -- The memory block to add the segment to
 *             (uint32_t) size -- Size of the segment to add
 * Return    : None
 * Notes     : Will CRE if mem is NULL
 */
SegmentID Mem_mapNew(Mem mem, uint32_t size) 
{
        
        if (mem -> nextSeenSize == 0) {
                if (mem -> segmentsCapacity == mem -> segmentsSize) {
                        expandSegments(mem);
                }
                mem -> segments[mem -> segmentsSize].words = CALLOC(size, sizeof(uint32_t));
                mem -> segments[mem -> segmentsSize].size  = size;

                uint32_t index = mem -> segmentsSize;
                mem -> segmentsSize++;
                return index;
        }
        mem -> nextSeenSize--;
        uint32_t nextPosition = mem -> nextSeenSize; 
        uint32_t index = mem -> nextSeenSegID[nextPosition];
        mem -> segments[index].words = CALLOC(size, sizeof(uint32_t)),
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
void Mem_mapFree(Mem mem, uint32_t segID) 
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
        
        uint32_t numInstructions = Seq_length(instructions);

        Mem mem = state -> mem;
        mem -> segments[0].words = CALLOC(numInstructions, sizeof(uint32_t));
        mem -> segments[0].size  = numInstructions;
        
        state -> mainInstructionSize = numInstructions;

        for (int index = 0; Seq_length(instructions) > 0; index++)
        {
                uint32_t instruction = 
                        (uint32_t)(uintptr_t) Seq_remlo(instructions);
                
                mem -> segments[0].words[index] = instruction;
        }

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
        // GET INSTRUCTION 
        if (state -> programCounter >= state -> mainInstructionSize) {
                state -> isRunning = false;
                return; 
        }
        uint32_t instruction = state -> mem -> segments[0].words[state -> programCounter]; // Mem_getWord(state -> mem, 0, state -> programCounter);
        state -> programCounter++;

        // END OF GET INSTRUCTION
        uint32_t instructionType = instruction & instructionBits;

        switch(instructionType) {
                case CMOV:
                // fprintf(stderr, "CMOV\n");
                CPU_cmove(state, instruction);
                break;

                case SEGLOAD:
                // fprintf(stderr, "SEGLOAD\n");
                CPU_segLoad(state, instruction);
                break;

                case SEGSTORE:
                // fprintf(stderr, "SEGSTORE\n");
                CPU_segStore(state, instruction);
                break;

                case ADD:
                // fprintf(stderr, "ADD r%u = r%u + r%u\n", ra, rb, rc);
                CPU_add(state, instruction);
                break;

                case MUL:
                // fprintf(stderr, "MUL\n");
                CPU_mult(state, instruction);
                break;

                case DIV:
                // fprintf(stderr, "DIV\n");
                CPU_div(state, instruction);
                break;

                case NAND:
                // fprintf(stderr, "NAND\n");
                CPU_nand(state, instruction);
                break;

                // case HALT:
                // return; 
                // break;

                case MAPSEG:
                // fprintf(stderr, "MAPSEG\n");
                CPU_mapSeg(state, instruction);
                break;

                case UNMAPSEG:
                // fprintf(stderr, "UNMAPSEG\n");
                CPU_unmapSeg(state, instruction);
                break;

                case OUT:
                // fprintf(stderr, "OUT\n");
                CPU_printOut(state, instruction);
                break;

                case IN:
                // fprintf(stderr, "IN\n");
                CPU_readIn(state, instruction);
                break;

                case LOADPROGRAM:
                // fprintf(stderr, "LOADPROGRAM\n");
                CPU_loadProgram(state, instruction);
                break;

                case LOADVALUE:
                // fprintf(stderr, "LOADVALUE r%u = %u\n", (instruction & rLoadBits) >> rLoadLsb, instruction & loadBits);
                CPU_loadValue(state, instruction);
                break;

                default:
                state -> isRunning = false;
                return; 
                break;
        }
        
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
 void CPU_cmove(CPU_State state, uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

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
 void CPU_segLoad(CPU_State state, uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        uint32_t *registers = state -> registers;
        Mem      mem        = state -> mem;
        registers[ra] = mem -> segments[registers[rb]].words[registers[rc]]; // Mem_getWord(mem, registers[rb], registers[rc]);
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
 void CPU_segStore(CPU_State state, uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        uint32_t *registers = state -> registers;
        Mem       mem       = state -> mem;
        mem -> segments[registers[ra]].words[registers[rb]] = registers[rc];
        // Mem_setWord(mem, registers[ra], registers[rb], registers[rc]);
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
 void CPU_add(CPU_State state, uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

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
 void CPU_mult(CPU_State state, uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

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
 void CPU_div(CPU_State state, uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

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
 void CPU_nand(CPU_State state, uint32_t instruction)
{
        uint32_t ra = (instruction & raBits) >> raLsb;
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        uint32_t *registers = state -> registers;
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
 void CPU_mapSeg(CPU_State state, uint32_t instruction)
{
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

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
 void CPU_unmapSeg(CPU_State state, uint32_t instruction)
{
        uint32_t rc = instruction & rcBits;

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
 void CPU_printOut(CPU_State state, uint32_t instruction)
{
        uint32_t rc = instruction & rcBits;

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
 void CPU_readIn(CPU_State state, uint32_t instruction)
{
        uint32_t rc = instruction & rcBits;

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
 void CPU_loadProgram(CPU_State state, uint32_t instruction)
{
        uint32_t rb = (instruction & rbBits) >> rbLsb;
        uint32_t rc = instruction & rcBits;

        uint32_t *registers = state -> registers;
        Mem      mem        = state -> mem;

        state -> programCounter = registers[rc];

        if (registers[rb] == 0) {
                return;
        }
        Segment seg = mem -> segments[registers[rb]];
        Segment copy = {
                CALLOC(seg.size, sizeof(uint32_t)),
                seg.size
        };
        FREE(mem -> segments[0].words);
        mem -> segments[0].words = copy.words;
        mem -> segments[0].size = copy.size;

        // Segment seg  = Mem_getSegment(mem, registers[rb]);
        // Segment copy = Segment_copy(seg);
        // state -> mainInstructionSize = Segment_size(copy);
        // // fprintf(stderr, "New Segment 0 Size: %i\n", Segment_size(copy));
        // Mem_setSegment(mem, 0, copy);
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


// /*
//  * Name      : Segment_new
//  * Purpose   : Creates new segment for the UM memory
//  * Parameter : (uint32_t) size -- The number of words in the segment
//  * Return    : (Segment) The struct representing a specific segment of memory
//  * Notes     : Will CRE if it does not have enough memory to allocate
//  */
// Segment Segment_new(uint32_t size)
// {
//         Segment seg  = ALLOC(sizeof(*seg));

//         seg -> size  = size;
//         if (size > 0) {
//                 seg -> words = CALLOC(size, sizeof(Word));
//         } 
//         else {
//                 seg -> words = NULL;
//         } 
        
//         return seg;
// }

// /*
//  * Name      : Segment_copy
//  * Purpose   : Creates a copy of the given segment
//  * Parameter : (Segment) seg -- The segment to copy
//  * Return    : (Segment) The struct representing the copy of the segment
//  * Notes     : Will CRE if the given segment is null;
//  *             Will CRE if it does not have enough memory to allocate
//  */
// Segment Segment_copy(Segment seg)
// {
//         uint32_t size = seg -> size;
//         Segment newSeg = Segment_new(size);

//         for (uint32_t wordIndex = 0; wordIndex < size; wordIndex++) {
//                 newSeg -> words[wordIndex] = seg -> words[wordIndex];
//         }

//         return newSeg;

// }

// /*
//  * Name      : Segment_free
//  * Purpose   : Frees segment of the UM memory
//  * Parameter : (Segment *) seg -- The pointer to the segment to free
//  * Return    : None
//  * Notes     : Will CRE if seg is null or *seg is null;
//  *             Will set the value in seg to NULL
//  */
// void Segment_free(Segment *seg)
// {
//         Segment segment = *seg;
        
//         if (segment -> words != NULL) {
//                 FREE(segment -> words);
//         }

//         FREE(*seg);
// }

// /*
//  * Name      : Segment_size
//  * Purpose   : Gets the size of the segment
//  * Parameter : (Segment) seg -- The segment to check the size of 
//  * Return    : (uint32_t) The size of the segment
//  * Notes     : Will CRE if seg is NULL
//  */
// uint32_t Segment_size(Segment seg)
// {
//         return seg -> size;
// }

// /*
//  * Name      : Segment_getWord
//  * Purpose   : Gets the word at a given position within the segment
//  * Parameter : (Segment)  seg      -- The segment to get word from
//  *             (uint32_t) position -- Position to access
//  * Return    : (Word)     The word that was located at the given position
//  * Notes     : Will CRE if seg is NULL or the position is greater than the 
//  *                 segment size
//  */
// Word Segment_getWord(Segment seg, uint32_t position)
// {
//         return seg -> words[position];
// }

// /*
//  * Name      : Segment_setWord
//  * Purpose   : Sets the word at a given position within the segment
//  * Parameter : (Segment)  seg      -- The segment to set word in
//  *             (uint32_t) position -- Position to access
//  *             (Word)     word     -- Word to place at the position
//  * Return    : None
//  * Notes     : Will CRE if seg is NULL or the position is greater than the 
//  *                 segment size
//  */
// void Segment_setWord(Segment seg, uint32_t position, Word word)
// {
//         seg -> words[position] = word;
// }

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
#undef START_SIZE