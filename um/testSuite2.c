#include "makeTests.h"
#include <seq.h>

/* 
void TEST(Seq_T stream)
{

}
*/

/* BATCH 1 */
void haltTest(Seq_T stream)
{
        halt(stream);
}

void haltAfterTest(Seq_T stream)
{
        halt(stream);
        cmove(stream, r0, r1, r2);
        halt(stream);
        loadValue(stream, r1, 'B');
        printOut(stream, r1);
        loadValue(stream, r1, 'a');
        printOut(stream, r1);
        loadValue(stream, r1, 'd');
        printOut(stream, r1);
        loadValue(stream, r1, '!');
        printOut(stream, r1);
        loadValue(stream, r1, '\n');
        printOut(stream, r1);
}

void addTest(Seq_T stream)
{
        loadValue(stream, r0, 1);
        loadValue(stream, r1, 2);
        add(stream, r2, r1, r0);
        printOut(stream, r2);
        halt(stream);
}

void printSizeTest(Seq_T stream)
{
        loadValue(stream, r0, 48);
        loadValue(stream, r1, 6);
        add(stream, r2, r1, r0);
        printOut(stream, r2);
        halt(stream);
}

void printAllCharactersTest(Seq_T stream)
{
        for (int i = 0; i < 255; i++) {
                loadValue(stream, i % 8, i);
                printOut(stream, i % 8);
        }
        halt(stream);
}

/* BATCH 2 */

void printGivenCharsTest(Seq_T stream)
{
        /*  creates r2 with all 1s (to compare with) */
        loadValue(stream, r2, 0x1ffffff);
        loadValue(stream, r3, 128);
        mult(stream, r2, r2, r3);
        add(stream, r2, r2, r3);

        /*  Creates the positions to jump to for the conditional moves */
        /*          r4 = normal, r5 = after loop, r6 = go back to the top */
        loadValue(stream, r4, 12);
        loadValue(stream, r5, 14);
        loadValue(stream, r6, 8);
        loadValue(stream, r7, 0);

        /* Read the input... (jump back with r6) */
        readInput(stream, r0);

        /* Put the not r0 into r3 */
        nand(stream, r3, r0, r0);
        
        /* If r0 was all 0s, then change the jump location to the end */
        cmove(stream, r4, r5, r3);
        loadProgram(stream, r7, r4);
        printOut(stream, r0);
        loadProgram(stream, r7, r6);
        halt(stream);
}