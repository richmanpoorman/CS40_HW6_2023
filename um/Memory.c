/* 
 *   Name       : Memory.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module representing the segmented memory of our UM; 
 *                supports setting and getting words and segments of memory
 *                and has knowledge of all mapped and unmapped segments
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include <stdint.h>
// #include <seq.h>
#include <mem.h>
#include <stdbool.h>
#include <stdlib.h>
#include "Segment.h"
#include <stdio.h>

#define START_SIZE 256

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


Mem Mem_new();
void Mem_freeMemory(Mem *mem);
Segment Mem_getSegment(Mem mem, uint32_t segID);
void Mem_setSegment(Mem mem, uint32_t segID, Segment seg);
Word Mem_getWord(Mem mem, uint32_t segID, uint32_t wordID);
void Mem_setWord(Mem mem, uint32_t segID, uint32_t wordID, 
                    uint32_t value);
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
                newSegments[i] = segments[i];
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
        mem -> segments[0]  = Segment_new(0);
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
                if (segments[i] == NULL) {
                        continue;
                } 
                Segment_free(&segments[i]);
        }

        FREE(memory -> segments);
        FREE(memory -> nextSeenSegID);
        FREE(*mem); 
}

/*
 * Name      : Mem_getSegment
 * Purpose   : Gets the full segment at a given memory id
 * Parameter : (Mem) mem -- The memory block containing the segment
 *             (uint32_t) segID -- The 32-bit memory identifier of the segment
 * Return    : None
 * Notes     : Will CRE if mem is NULL or the segID is greater than the 
 *             memory size
 */
Segment Mem_getSegment(Mem mem, uint32_t segID) 
{
        
        return mem -> segments[segID];
}

/*
 * Name      : Mem_setSegment
 * Purpose   : Sets the segment in memory to a given value
 * Parameter : (Mem) mem -- The memory block containing the segment
 *             (uint32_t) segID -- The 32-bit memory identifier of the segment
 *             (Segment) seg -- The value to set the segment to
 * Return    : None
 * Notes     : Will CRE if mem is NULL 
 */
void Mem_setSegment(Mem mem, uint32_t segID, Segment seg) 
{
        Segment_free(&(mem -> segments[segID]));
        mem -> segments[segID] = seg;
        
}

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
Word Mem_getWord(Mem mem, uint32_t segID, uint32_t wordID) 
{
        
        return Segment_getWord(mem -> segments[segID], wordID);
}

/*
 * Name      : Mem_setWord
 * Purpose   : Sets the word at a given position in a given memory segment
 * Parameter : (Mem) mem -- The memory block containing the segment
 *             (uint32_t) segID -- The memory identifier of the segment
 *             (uint32_t) wordID -- The word identifier within the segment
 *             (uint32_t) value -- The value to set the word to
 * Return    : None
 * Notes     : Will CRE if mem is NULL or the segID is greater than the 
 *             memory size
 */
void Mem_setWord(Mem mem, uint32_t segID, uint32_t wordID, 
                    uint32_t value) 
{
        Segment_setWord(mem -> segments[segID], wordID, value);
}

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
                mem -> segments[mem -> segmentsSize] = Segment_new(size);
                uint32_t index = mem -> segmentsSize;
                mem -> segmentsSize++;
                return index;
        }
        mem -> nextSeenSize--;
        uint32_t nextPosition = mem -> nextSeenSize; 
        uint32_t index = mem -> nextSeenSegID[nextPosition];
        mem -> segments[index] = Segment_new(size);
        return index;

        // /* uses an unused segment ID if they exist in memory */
        // if (Seq_length(mem -> nextSeenSegID) > 0) { 
        //         uint32_t index = 
        //                 (uint32_t)(uintptr_t)Seq_remlo(mem -> nextSeenSegID);
        //         Seq_put(mem -> segments, index, Segment_new(size));
        //         return index; 
        // }
        // else { /* otherwise creates a completely new segment ID */
        //         Seq_addhi(mem -> segments, Segment_new(size));
        // }
        // return Seq_length(mem -> segments) - 1;
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
        
        Segment_free(&(mem -> segments[segID]));
        uint32_t nextPos = mem -> nextSeenSize;
        if (mem -> nextSeenCapacity == nextPos) {
                expandNextSeen(mem);
        }
        mem -> nextSeenSegID[nextPos] = segID;
        mem -> nextSeenSize++;
        // Segment seg = Seq_get(mem -> segments, segID);
        // Segment_free(&seg);
        // Seq_put(mem -> segments, segID, NULL);
        // Seq_addhi(mem -> nextSeenSegID, (void *)(uintptr_t)segID);
}
#undef START_SIZE