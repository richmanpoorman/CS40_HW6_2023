/* 
 *   Name       : Memory.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module representing the segmented memory of our UM; 
 *                supports setting and getting words and segments of memory
 *                and has knowledge of all mapped and unmapped segments
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include <stdint.h>
#include <stdbool.h>
#include "Segment.h"

#ifndef MEMORY_H_
#define MEMORY_H_


typedef struct Mem *Mem;
typedef uint32_t SegmentID;

Mem Mem_new();

void Mem_freeMemory(Mem *mem);

Segment Mem_getSegment(Mem mem, uint32_t segID);

void Mem_setSegment(Mem mem, uint32_t segID, Segment seg);

Word Mem_getWord(Mem mem, uint32_t segID, uint32_t wordID);

void Mem_setWord(Mem mem, uint32_t segID, uint32_t wordID, 
                    uint32_t value);

SegmentID Mem_mapNew(Mem mem, uint32_t size);

void Mem_mapFree(Mem mem, uint32_t segID);

bool Mem_isInRange(Mem mem, uint32_t segID, uint32_t wordID);

#endif