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
void jumpOnZeroTest(Seq_T stream)
{
        /* r7 has 0, r6 has -1 */
        loadValue(stream, r7, 0);
        nand(stream, r6, r7, r7);
        loadValue(stream, r5, 1);

        loadValue(stream, r0, 255);

        loadValue(stream, r3, 6);
        loadValue(stream, r2, 10);

        loadValue(stream, r1, 9);
        cmove(stream, r1, r2, r0);
        loadProgram(stream, r7, r1);
        halt(stream);
        
        printOut(stream, r0);
        add(stream, r0, r0, r6);
        loadProgram(stream, r7, r3);
}

void printLetters(Seq_T stream)
{
        loadValue(stream, r0, 97);
        loadValue(stream, r7, 122);
        loadValue(stream, r6, 1);
        loadValue(stream, r5, 0);

        loadValue(stream, r3, 5); /* Jump to output value */

        printOut(stream, r0);
        loadValue(stream, r1, 13); /* Jump to halt value */

        /* Check that r7 - r0 */
        nand(stream, r2, r0, r0);
        add(stream, r2, r2, r6);
        add(stream, r2, r2, r7);

        /* If r7 - r0 != 0, go back up */
        cmove(stream, r1, r3, r2);
        add(stream, r0, r0, r6);
        loadProgram(stream, r5, r1);

        /* UPPER CASE */
        loadValue(stream, r0, 65);
        loadValue(stream, r7, 90);
        loadValue(stream, r6, 1);
        loadValue(stream, r5, 0);

        loadValue(stream, r3, 18); /* Jump to output value */

        printOut(stream, r0);
        loadValue(stream, r1, 26); /* Jump to halt value */

        /* Check that r7 - r0 */
        nand(stream, r2, r0, r0);
        add(stream, r2, r2, r6);
        add(stream, r2, r2, r7);

        /* If r7 - r0 != 0, go back up */
        cmove(stream, r1, r3, r2);
        add(stream, r0, r0, r6);
        loadProgram(stream, r5, r1);

        halt(stream);
}

void printGivenCharsTest(Seq_T stream)
{

        /*  Creates the positions to jump to for the conditional moves */
        /*          r4 = normal, r5 = after loop, r6 = go back to the top */
        loadValue(stream, r4, 9);
        loadValue(stream, r5, 11);
        loadValue(stream, r6, 5);
        loadValue(stream, r7, 0);
        loadValue(stream, r1, 1);

        /* Read the input... (jump back with r6) */
        readIn(stream, r0);

        /* Put the not r0 into r3 */
        add(stream, r3, r0, r1);
        
        /* If r0 was all 0s, then change the jump location to the end */
        cmove(stream, r4, r5, r3);
        loadProgram(stream, r7, r4);
        halt(stream);
        printOut(stream, r0);
        loadProgram(stream, r7, r6);
        
}
