/* 
 *   Name       : Mem.h
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module representing the memory of our Universal Machine
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#ifndef MEMORY_H_
#define MEMORY_H_
#include <stdint.h>
#include "Segment.h"

typedef struct Mem *Mem;
typedef Word uint32_t;
typedef SegmentID uint32_t

Mem Mem_new();

void Mem_free(Mem *mem);

Segment Mem_getSegment(Mem mem, uint32_t memID);

void Mem_setSegment(Mem mem, uint32_t memID, Segment seg);

Word Mem_getWord(Mem mem, uint32_t segID, uint32_t wordID);

void Mem_setWord(Mem mem, uint32_t segID, uint32_t wordID, 
                    uint32_t value);

SegmentID Mem_mapNew(Mem mem);

void Mem_mapFree(Mem mem, uint32_t segID);

