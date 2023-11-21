#include "makeTests.h"
#include <seq.h>

/* 
void TEST(Seq_T stream)
{

}
*/

/* BATCH 1 */
/* Halts */
void haltTest(Seq_T stream)
{
        halt(stream);
}
/* Checks it only runs halt */
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
/* Adds two numbers together */
void addTest(Seq_T stream)
{
        loadValue(stream, r0, 1);
        loadValue(stream, r1, 2);
        add(stream, r2, r1, r0);
        printOut(stream, r2);
        halt(stream);
}

/* Prints out a 6 (as 48 + 6 = 56, which is '6') */
void printSizeTest(Seq_T stream)
{
        loadValue(stream, r0, 48);
        loadValue(stream, r1, 6);
        add(stream, r2, r1, r0);
        printOut(stream, r2);
        halt(stream);
}

/* Print out 255 characters (without jumps) */
void printAllCharactersTest(Seq_T stream)
{
        for (int i = 0; i < 255; i++) {
                loadValue(stream, i % 8, i);
                printOut(stream, i % 8);
        }
        halt(stream);
}

/* BATCH 2 */
/* Prints all 255 characters using an UM "loop"*/
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

/* Prints a...z then A...Z */
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
/* Batch 3 */
/* Tries to read back all of the data */
void printGivenCharsTest(Seq_T stream)
{
        loadValue(stream, r7, 0); /* 0 */
        loadValue(stream, r6, 3); /* Jump back up */
        loadValue(stream, r5, 8); /* Jump to next */

        readIn(stream, r0);
        nand(stream, r1, r0, r0);
        loadValue(stream, r2, 10); /* Jump to halt */
        cmove(stream, r2, r5, r1);
        loadProgram(stream, r7, r2);
        printOut(stream, r0);
        loadProgram(stream, r7, r6);
        halt(stream);
        
}

/* Tries to take "hi!" and responds "ok" if done*/
void takeInputTest(Seq_T stream)
{       
        loadValue(stream, r2, 28); /* Halt instruction */
        loadValue(stream, r4, 0); /* r4 = 0 */
        loadValue(stream, r5, 1); /* r5 = 1 */

        /* -('H') */
        loadValue(stream, r0, 'H'); 
        nand(stream, r0, r0, r0);
        add(stream, r0, r0, r5);

        readIn(stream, r1);
        
        loadValue(stream, r3, 10); 
        cmove(stream, r3, r2, r0);
        loadProgram(stream, r4, r3);

        /* -('i') */
        loadValue(stream, r0, 'i'); 
        nand(stream, r0, r0, r0);
        add(stream, r0, r0, r5);

        readIn(stream, r1);
        
        loadValue(stream, r3, 17); 
        cmove(stream, r3, r2, r0);
        loadProgram(stream, r4, r3);

        /* -('!') */
        loadValue(stream, r0, '!'); 
        nand(stream, r0, r0, r0);
        add(stream, r0, r0, r5);

        readIn(stream, r1);
        
        loadValue(stream, r3, 24); 
        cmove(stream, r3, r2, r0);
        loadProgram(stream, r4, r3);

        loadValue(stream, r0, 'o'); 
        printOut(stream, r0);
        loadValue(stream, r0, 'k'); 
        printOut(stream, r0);
        
        halt(stream);
}
/* Run instructions before halting, printing "ok" and not "ok NO!"*/
void instructionsBeforeHalt(Seq_T stream)
{
        loadValue(stream, r0, 'o');
        printOut(stream, r0);

        loadValue(stream, r0, 'k');
        printOut(stream, r0);

        halt(stream);

        loadValue(stream, r0, ' ');
        printOut(stream, r0);

        loadValue(stream, r0, 'N');
        printOut(stream, r0);

        loadValue(stream, r0, 'O');
        printOut(stream, r0);

        loadValue(stream, r0, '!');
        printOut(stream, r0);
        halt(stream);
}

/* Checks possible nands between 0 and 0xffffffff */
void nandTest(Seq_T stream)
{
        loadValue(stream, r7, 26); /* GO TO BAD */
        loadValue(stream, r6, 0); /* 0 */


        loadValue(stream, r5, 0); /* INT_MAX (if correct) */
        nand(stream, r5, r0, r0); 
        
        loadValue(stream, r2, 1);
        add(stream, r2, r2, r5);

        loadValue(stream, r3, 9); /* GO TO NEXT */
        cmove(stream, r3, r7, r2);
        loadProgram(stream, r6, r3);

        nand(stream, r1, r6, r5); /* INT_MAX with 0 */
        
        loadValue(stream, r2, 1);
        add(stream, r2, r2, r1);

        loadValue(stream, r3, 15); /* GO TO NEXT */
        cmove(stream, r3, r7, r2);
        loadProgram(stream, r6, r3);

        nand(stream, r1, r5, r5); /* INT_MAX with INT_MAX */

        loadValue(stream, r3, 19); /* GO TO NEXT */
        cmove(stream, r3, r7, r1);
        loadProgram(stream, r6, r3);

        loadValue(stream, r0, 'Y');
        printOut(stream, r0);
        loadValue(stream, r0, 'A');
        printOut(stream, r0);
        loadValue(stream, r0, 'Y');
        printOut(stream, r0);

        halt(stream);

        loadValue(stream, r0, 'B');
        printOut(stream, r0);
        loadValue(stream, r0, 'A');
        printOut(stream, r0);
        loadValue(stream, r0, 'D');
        printOut(stream, r0);
        halt(stream);
}

/* Create new segments of size 0 and 4, set/get from size 4, and free */
void segTest(Seq_T stream) 
{
        
        loadValue(stream, r7, 0); /* GO TO BAD */
        loadValue(stream, r6, 1);

        loadValue(stream, r0, 0);
        mapSeg(stream, r1, r0);

        loadValue(stream, r0, 4);
        mapSeg(stream, r2, r0);
        
        for (int i = 0; i < 4; i++) {
                loadValue(stream, r3, 'a' + i);
                loadValue(stream, r0, i);
                segStore(stream, r2, r0, r3);
                segLoad(stream, r4, r2, r0);
                printOut(stream, r4);
        }
        
        unmapSeg(stream, r1);
        unmapSeg(stream, r2);

        halt(stream);
}

