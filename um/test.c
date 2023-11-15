

#include <stdio.h>
#include <assert.h>
#include <except.h>
#include <stdbool.h>

#include "Segment.h"

#define CHECK(VALUE) { if (!(VALUE)) { return false; } }

typedef bool (*TestFunction)();

/*
 * Name       : segmentNewTest
 * Purpose    : Create a new segment and check that it is not null
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_new
 */
bool segmentNewTest()
{
        Segment seg = Segment_new(100);

        CHECK(seg != NULL);

        Segment_free(&seg);

        return true;
}
/*
 * Name       : segmentFreeTest
 * Purpose    : Frees a segment and checks that the segment is set to 
                null without memory leaks
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses sSegment_free
 */
bool segmentFreeTest()
{
        Segment seg = Segment_new(100);

        Segment_free(&seg);
        
        CHECK(seg == NULL);

        return true;
}
/*
 * Name       : segmentSetZero
 * Purpose    : Set the 0th word to 100 and check the 0th word is 100
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_setWord and Segment_getWord
 */
bool segmentSetZero()
{
        Segment seg = Segment_new(100);

        Segment_setWord(seg, 0, 100);

        CHECK(Segment_getWord(seg, 0) == 100);

        Segment_free(&seg);

        return true;
}
/*
 * Name       : segmentSetLast
 * Purpose    : Set the 2^32 - 1th word to 100 and check the 2^32 -               
                1th word is 100
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_setWord and Segment_getWord
 */
bool segmentSetLast()
{
        uint32_t maxValue = 0;
        maxValue = ~maxValue;
        Segment seg = Segment_new(maxValue);

        Segment_setWord(seg, maxValue - 1, 100);

        CHECK(Segment_getWord(seg, maxValue - 1) == 100);

        Segment_free(&seg);

        return true;
}
/*
 * Name       : segmentCheckThree
 * Purpose    : Set the 0th and 2nd word to 0, and set 1st word to 
                2^32 - 1 (check words don’t impact each other) and 
                check they have 0, 2^32 - 1, 0 respectively
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_setWord and Segment_getWord
 */
bool segmentCheckThree()
{
        uint32_t maxValue = 0;
        maxValue = ~maxValue;
        Segment seg = Segment_new(3);
        Segment_setWord(seg, 0, 0);
        Segment_setWord(seg, 1, maxValue);
        Segment_setWord(seg, 2, 0);

        CHECK(Segment_getWord(seg, 0) == 0);
        CHECK(Segment_getWord(seg, 1) == maxValue);
        CHECK(Segment_getWord(seg, 2) == 0);

        Segment_free(&seg);

        return true;
}
/*
 * Name       : segmentCheckThreeAlt
 * Purpose    : Set the 0th and 2nd word to  2^32 - 1 and set the 1st 
                word to 0  (check words don’t impact each other) and 
                check they have 2^32 - 1, 0, 2^32 - 1 respectively
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_setWord and Segment_getWord
 */
bool segmentCheckThreeAlt()
{
        uint32_t maxValue = 0;
        maxValue = ~maxValue;
        Segment seg = Segment_new(3);
        Segment_setWord(seg, 0, maxValue);
        Segment_setWord(seg, 1, 0);
        Segment_setWord(seg, 2, maxValue);

        CHECK(Segment_getWord(seg, 0) == maxValue);
        CHECK(Segment_getWord(seg, 1) == 0);
        CHECK(Segment_getWord(seg, 2) == maxValue);

        Segment_free(&seg);

        return true;
}
/*
 * Name       : segmentBoundsFailure
 * Purpose    : Attempt to set and get an out of bounds segment;   
                Create segment of length 15 and set/get 16th word
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_setWord and Segment_getWord
 */
bool segmentBoundsFailure()
{
        Segment seg = Segment_new(15);
        Segment_getWord(seg, 16);
        Segment_setWord(seg, 16, 0);

        Segment_free(&seg);
        return false;
}
/*
 * Name       : segmentZeroFailure
 * Purpose    : Create a segment of length zero and attempt to set 
                Segment_setWord and get from the segment
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_setWord and Segment_getWord
 */
bool segmentZeroFailure()
{
        Segment seg = Segment_new(0);
        Segment_getWord(seg, 0);
        Segment_setWord(seg, 0, 0);

        Segment_free(&seg);
        return false;
}
/*
 * Name       : segmentLengthOne
 * Purpose    : Create segment of length 1 and set 0th word
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_setWord and Segment_getWord
 */
bool segmentLengthOne()
{
        Segment seg = Segment_new(1);
        Segment_setWord(seg, 0, 25);
        CHECK(Segment_getWord(seg, 0) == 25);

        Segment_free(&seg);
        return true;
}
/*
 * Name       : segmentLengthThree
 * Purpose    : Create a segment of length 3, and set the words to 1, 
                2, 3 respectively, and get all the words being 1, 2, 
                3 respectively
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_setWord and Segment_getWord
 */
bool segmentLengthThree()
{
        Segment seg = Segment_new(3);
        Segment_setWord(seg, 0, 1);
        Segment_setWord(seg, 1, 2);
        Segment_setWord(seg, 2, 3);

        CHECK(Segment_getWord(seg, 0) == 1);
        CHECK(Segment_getWord(seg, 1) == 2);
        CHECK(Segment_getWord(seg, 2) == 3);

        Segment_free(&seg);
        return true;
}
/*
 * Name       : segmentCopyTest
 * Purpose    : Make a segment, then copy and check the addresses are 
                not the same
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_copy, Segment_setWord, and 
                Segment_getWord
 */
bool segmentCopyTest()
{
        Segment seg = Segment_new(3);
        Segment_setWord(seg, 0, 1);
        Segment_setWord(seg, 1, 2);
        Segment_setWord(seg, 2, 3);

        Segment copy = Segment_copy(seg);
        Segment_setWord(copy, 0, 4);
        Segment_setWord(copy, 1, 5);
        Segment_setWord(copy, 2, 6);

        CHECK(Segment_getWord(seg, 0) == 1);
        CHECK(Segment_getWord(seg, 1) == 2);
        CHECK(Segment_getWord(seg, 2) == 3);
        CHECK(Segment_getWord(copy, 0) == 4);
        CHECK(Segment_getWord(copy, 1) == 5);
        CHECK(Segment_getWord(copy, 2) == 6);
        CHECK(Segment_size(copy) == 3);


        Segment_free(&seg);
        Segment_free(&copy);
        return true;
}
/*
 * Name       : segmentCopyZero
 * Purpose    : Make a segment of length 0 and copy, then check that 
                a new copy of length 0 was created
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_copy and Segment_size
 */
bool segmentCopyZero()
{
        Segment seg = Segment_new(0);

        Segment copy = Segment_copy(seg);

        
        CHECK(Segment_size(seg) == 0);
        CHECK(Segment_size(copy) == 0);

        Segment_free(&seg);
        Segment_free(&copy);
        return true;
}
/*
 * Name       : segmentCopyTwo
 * Purpose    : Make a segment of size 2 and check that the copy also 
                has same size
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_copy, Segment_setWord, Segment_getWord,
 *              and Segment_size
 */
bool segmentCopyTwo()
{
        Segment seg = Segment_new(2);
        Segment_setWord(seg, 0, 1);
        Segment_setWord(seg, 1, 2);

        Segment copy = Segment_copy(seg);
        
        CHECK(Segment_getWord(copy, 0) == 1);
        CHECK(Segment_getWord(copy, 1) == 2);
        CHECK(Segment_size(copy) == 2);

        Segment_free(&seg);
        Segment_free(&copy);
        return true;
}
/*
 * Name       : segmentCopyAndMod
 * Purpose    : Make a segment of size 2 and set the values to 1, 2 
                then make a copy and change the values to 3, 4, then 
                check that the original segment is unaltered while 
                the copy is altered
 * Parameters : None
 * Return     : (bool) True if the test passes, false otherwise
 * Notes      : Uses Segment_copy, Segment_setWord, and 
                Segment_getWord
 */
bool segmentCopyAndMod()
{
        Segment seg = Segment_new(2);
        Segment_setWord(seg, 0, 1);
        Segment_setWord(seg, 1, 2);

        Segment copy = Segment_copy(seg);
        Segment_setWord(copy, 0, 3);
        Segment_setWord(copy, 1, 4);
        
        CHECK(Segment_getWord(seg, 0) == 1);
        CHECK(Segment_getWord(seg, 1) == 2);
        CHECK(Segment_getWord(copy, 0) == 3);
        CHECK(Segment_getWord(copy, 1) == 4);
        CHECK(Segment_size(copy) == 2);

        Segment_free(&seg);
        Segment_free(&copy);
        return true;
}


bool tryOrCatch(bool (*testFunction)())
{
        TRY 
                RETURN testFunction();
        FINALLY
                RETURN false;
        END_TRY;
}

int main(int argc, char *argv[])
{
        
        TestFunction testFunctions[] = {
                segmentNewTest,
                segmentFreeTest, 
                segmentSetZero,
                
                segmentCheckThree,
                segmentCheckThreeAlt,
                
                segmentLengthOne,
                segmentLengthThree,
                segmentCopyTest,
                segmentCopyZero,
                segmentCopyTwo,
                segmentCopyAndMod
        };
        char *functionNames[] = {
                "segmentNewTest",
                "segmentFreeTest", 
                "segmentSetZero",
                
                "segmentCheckThree",
                "segmentCheckThreeAlt",

                "segmentLengthOne",
                "segmentLengthThree",
                "segmentCopyTest",
                "segmentCopyZero",
                "segmentCopyTwo",
                "segmentCopyAndMod"
        };

        TestFunction failureTests[] = {
                segmentSetLast,
                segmentBoundsFailure,
                segmentZeroFailure
        };
        char *failFunctionNames[] = {
                "segmentSetLast",
                "segmentBoundsFailure",
                "segmentZeroFailure"
        };
        (void) failureTests;
        (void) failFunctionNames;

        int s1 = sizeof(testFunctions) / sizeof(*testFunctions);
        int s2 = sizeof(functionNames) / sizeof(*functionNames);

        assert(s1 == s2);
        
        for (int i = 0; i < s1; i++) {
                fprintf(stderr, "%s: ", functionNames[i]);
                if (tryOrCatch(testFunctions[i])) {
                        fprintf(stderr, "PASSED\n");
                }
                else {
                        fprintf(stderr, "FAILED\n");
                }
        }

        (void) argc;
        (void) argv;
}

#undef CHECK