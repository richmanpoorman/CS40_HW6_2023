/* 
 *   Name       : Segment.h
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module representing a single segment of the UM's memory,
 *                supports setting and getting specific words of a segment
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include <stdint.h>

#ifndef SEGMENT_H_
#define SEGMENT_H_

typedef uint32_t Word; 
typedef struct Segment *Segment;


Segment Segment_new(uint32_t size);

Segment Segment_copy(Segment seg);

void Segment_free(Segment *seg);

uint32_t Segment_size(Segment seg);

Word Segment_getWord(Segment seg, uint32_t position);

void Segment_setWord(Segment seg, uint32_t position, Word word);

#endif