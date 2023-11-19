#include <stdlib.h>
#include <stdint.h>
#include <mem.h>
#include <assert.h>

typedef uint32_t Word;
typedef struct Segment {
        Word *words;
        uint32_t size;
} *Segment;

Segment Segment_new();
Segment Segment_copy(Segment seg);
void Segment_free(Segment *seg);
uint32_t Segment_size(Segment seg);
Word Segment_getWord(Segment seg, uint32_t position);
void Segment_setWord(Segment seg, uint32_t position, Word word);

/*
 * Name      : Segment_new
 * Purpose   : Creates new segment for the UM memory
 * Parameter : (uint32_t) size -- The number of words in the segment
 * Return    : (Segment) The struct representing a specific segment of memory
 * Notes     : Will CRE if it does not have enough memory to allocate
 */
Segment Segment_new(uint32_t size)
{
        Segment seg  = ALLOC(sizeof(*seg));

        seg -> size  = size;
        if (size > 0) {
                seg -> words = CALLOC(size, sizeof(Word));
        } 
        else {
                seg -> words = NULL;
        } 
        
        return seg;
}

/*
 * Name      : Segment_copy
 * Purpose   : Creates a copy of the given segment
 * Parameter : (Segment) seg -- The segment to copy
 * Return    : (Segment) The struct representing the copy of the segment
 * Notes     : Will CRE if the given segment is null;
 *             Will CRE if it does not have enough memory to allocate
 */
Segment Segment_copy(Segment seg)
{
        assert(seg != NULL);
        uint32_t size = seg -> size;
        Segment newSeg = Segment_new(size);

        for (uint32_t wordIndex = 0; wordIndex < size; wordIndex++) {
                newSeg -> words[wordIndex] = seg -> words[wordIndex];
        }

        return newSeg;

}

/*
 * Name      : Segment_free
 * Purpose   : Frees segment of the UM memory
 * Parameter : (Segment *) seg -- The pointer to the segment to free
 * Return    : None
 * Notes     : Will CRE if seg is null or *seg is null;
 *             Will set the value in seg to NULL
 */
void Segment_free(Segment *seg)
{
        assert(seg != NULL && *seg != NULL);
        
        Segment segment = *seg;
        
        if (segment -> words != NULL) {
                FREE(segment -> words);
        }

        FREE(*seg);
}

/*
 * Name      : Segment_size
 * Purpose   : Gets the size of the segment
 * Parameter : (Segment) seg -- The segment to check the size of 
 * Return    : (uint32_t) The size of the segment
 * Notes     : Will CRE if seg is NULL
 */
uint32_t Segment_size(Segment seg)
{
        assert(seg != NULL);
        return seg -> size;
}

/*
 * Name      : Segment_getWord
 * Purpose   : Gets the word at a given position within the segment
 * Parameter : (Segment)  seg      -- The segment to get word from
 *             (uint32_t) position -- Position to access
 * Return    : (Word)     The word that was located at the given position
 * Notes     : Will CRE if seg is NULL or the position is greater than the 
 *                 segment size
 */
Word Segment_getWord(Segment seg, uint32_t position)
{
        assert(seg != NULL && seg -> words != NULL && position < seg -> size);
        return seg -> words[position];
}

/*
 * Name      : Segment_setWord
 * Purpose   : Sets the word at a given position within the segment
 * Parameter : (Segment)  seg      -- The segment to set word in
 *             (uint32_t) position -- Position to access
 *             (Word)     word     -- Word to place at the position
 * Return    : None
 * Notes     : Will CRE if seg is NULL or the position is greater than the 
 *                 segment size
 */
void Segment_setWord(Segment seg, uint32_t position, Word word)
{
        assert(seg != NULL && seg -> words != NULL && position < seg -> size);
        seg -> words[position] = word;
}