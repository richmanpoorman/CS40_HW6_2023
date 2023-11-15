#include <stdio.h>
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


Segment Segment_new(uint32_t size)
{
        Segment seg  = ALLOC(sizeof(*seg));

        seg -> size  = size;
        seg -> words = CALLOC(size, sizeof(Word));

        return seg;
}

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

void Segment_free(Segment *seg)
{
        assert(seg != NULL && *seg != NULL);
        FREE((*seg) -> words);
        FREE(seg);
}

uint32_t Segment_size(Segment seg)
{
        assert(seg != NULL);
        return seg -> size;
}

Word Segment_getWord(Segment seg, uint32_t position)
{
        assert(seg != NULL && position < seg -> size);
        return seg -> words[position];
}

void Segment_setWord(Segment seg, uint32_t position, Word word)
{
        assert(seg != NULL && position < seg -> size);
        seg -> words[position] = word;
}