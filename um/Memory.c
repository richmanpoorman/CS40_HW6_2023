/* 
 *   Name       : Memory.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module representing the segmented memory of our UM; 
 *                supports setting and getting words and segments of memory
 *                and has knowledge of all mapped and unmapped segments
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include <stdint.h>
#include <seq.h>
#include <mem.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include "Segment.h"

typedef uint32_t SegmentID;
typedef struct Mem {
        Seq_T segments;
        Seq_T nextSeenSegID;
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
bool Mem_isInRange(Mem mem, uint32_t segID, uint32_t wordID);

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
        mem -> segments = Seq_new(1000);   
        Seq_addhi(mem -> segments, Segment_new(0));
        mem -> nextSeenSegID = Seq_new(50);

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
        assert(mem != NULL && *mem != NULL);
        Mem memory = *mem;

        Seq_T segments = memory -> segments;
        
        while (Seq_length(segments) > 0) {
                Segment seg = Seq_remlo(segments);
                if (seg != NULL) {
                        Segment_free(&seg);
                }
        }
        Seq_free(&(memory -> segments));

        Seq_free(&(memory -> nextSeenSegID));
        
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
        assert(mem != NULL && segID < (uint32_t)Seq_length(mem -> segments));
        return Seq_get(mem -> segments, segID);
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
        assert(mem != NULL && segID < (uint32_t)Seq_length(mem -> segments));
        Segment prevSegment = Mem_getSegment(mem, segID);
        Segment_free(&prevSegment);
        Seq_put(mem -> segments, segID, seg);
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
        assert(mem != NULL && segID < (uint32_t)Seq_length(mem -> segments));
        Segment seg = Mem_getSegment(mem, segID);
        return Segment_getWord(seg, wordID);
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
        assert(mem != NULL && segID < (uint32_t)Seq_length(mem -> segments));
        Segment seg = Mem_getSegment(mem, segID);
        Segment_setWord(seg, wordID, value);
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
        assert(mem != NULL);
        /* uses an unused segment ID if they exist in memory */
        if (Seq_length(mem -> nextSeenSegID) > 0) { 
                uint32_t index = 
                        (uint32_t)(uintptr_t)Seq_remlo(mem -> nextSeenSegID);
                Seq_put(mem -> segments, index, Segment_new(size));
                return index; 
        }
        else { /* otherwise creates a completely new segment ID */
                Seq_addhi(mem -> segments, Segment_new(size));
        }
        return Seq_length(mem -> segments) - 1;
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
        assert(mem != NULL && segID < (uint32_t)Seq_length(mem -> segments));
        Segment seg = Seq_get(mem -> segments, segID);
        Segment_free(&seg);
        Seq_put(mem -> segments, segID, NULL);
        Seq_addhi(mem -> nextSeenSegID, (void *)(uintptr_t)segID);
}

/*
 * Name      : Mem_isInRange
 * Purpose   : Determines whether a given word of a given segment is mapped in
 *             the memory of our Universal Machine
 * Parameter : (Mem) mem -- The memory block with the segment to free
 *             (Segment) seg -- The 32-bit memory identifier of the segment
 * Return    : True if $m[segID][wordID] is mapped, false otherwise
 * Notes     : Will CRE if mem is NULL
 */
bool Mem_isInRange(Mem mem, uint32_t segID, uint32_t wordID) 
{
        /* checks that segID is in bounds*/
        if (segID >= (uintptr_t)Seq_length(mem -> segments)) { 
                return false;
        }
        /* checks that wordID is in bounds*/
        if (wordID >= Segment_size(Mem_getSegment(mem, segID))) {
                return false;
        }
        return true;
}
