/* 
 *   Name       : testsSuite1.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Basic mathematical unit tests for the UM
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include "makeTests.h"
#include <seq.h>

/* Add two zeros together
   Expected result: 0 */
void add_twoZerosTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 0);
        loadValue(sequence, r2, 0);
        add(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Add two nonzero numbers together
   Expected result: 99 */
void add_twoNonzerosTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 64);
        loadValue(sequence, r2, 35);
        add(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Add three nonzero numbers together
   Expected result: 163 */
void add_threeNumsTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 105);
        loadValue(sequence, r2, 13);
        add(sequence, r0, r1, r2);
        loadValue(sequence, r3, 45);
        add(sequence, r4, r0, r3);
        printOut(sequence, r4);
        halt(sequence); 
}

/* Add register to itself
   Expected result: 154 */
void add_toSelfTest(Seq_T sequence) 
{
        loadValue(sequence, r0, 77);
        add(sequence, r0, r0, r0);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Multiplies two zeros together
   Expected result: 0 */
void mult_twoZerosTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 0);
        loadValue(sequence, r2, 0);
        mult(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Multiplies two nonzeros together
   Expected result: 180 */
void mult_twoNonzerosTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 36);
        loadValue(sequence, r2, 5);
        mult(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Multiplies three nonzeros together
   Expected result: 240 */
void mult_threeNumsTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 4);
        loadValue(sequence, r2, 20);
        mult(sequence, r0, r1, r2);
        loadValue(sequence, r3, 3);
        mult(sequence, r4, r0, r3);
        printOut(sequence, r4);
        halt(sequence); 
}

/* Multiplies register by itself
   Expected result: 25 */
void mult_toSelfTest(Seq_T sequence) 
{
        loadValue(sequence, r0, 5);
        mult(sequence, r0, r0, r0);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Divides even number by 1
   Expected result: 54 */
void div_evenIdentityTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 54);
        loadValue(sequence, r2, 1);
        divide(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Divides odd number by 1
   Expected result: 55 */
void div_oddIdentityTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 55);
        loadValue(sequence, r2, 1);
        divide(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Divides even number by 2
   Expected result: 27 */
void div_evenByTwoTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 54);
        loadValue(sequence, r2, 2);
        divide(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Divides odd number by 2
   Expected result: 27 */
void div_oddByTwoTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 55);
        loadValue(sequence, r2, 2);
        divide(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Divides number by itself
   Expected result: 105 */
void div_bySelfTest(Seq_T sequence) 
{
        loadValue(sequence, r0, 105);
        divide(sequence, r0, r0, r0);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Cmoves based on a register that contains 0
   Expected result: Cmove proceeds, r0 contains 100*/
void cmove_moveTest(Seq_T sequence) 
{
        loadValue(sequence, r0, 50);
        loadValue(sequence, r1, 100);
        loadValue(sequence, r2, 0);
        cmove(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* Cmoves based on a register that does not contain 0
   Expected result: Cmove does not proceed, r0 contains 50*/
void cmove_stayTest(Seq_T sequence) 
{
        loadValue(sequence, r0, 50);
        loadValue(sequence, r1, 100);
        loadValue(sequence, r2, 200);
        cmove(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* hi tas */
/* no good naughty tests!!! these will fail on purpose.. DO NOT SUBMIT */

/* tries to load a value that is too large and add to it*/
void add_overflowTest(Seq_T sequence) 
{
        uint32_t maxValue = 0;
        maxValue = ~maxValue;
        loadValue(sequence, r1, 77);
        loadValue(sequence, r2, maxValue - 1);
        add(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* tries to load a value that is too large and multiply it */
void mult_overflowTest(Seq_T sequence) 
{
        uint32_t maxValue = 0;
        maxValue = ~maxValue;
        loadValue(sequence, r1, 30);
        loadValue(sequence, r2, maxValue - 1);
        mult(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* divides by zero mwahahahaha */
void div_zeroTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 0);
        loadValue(sequence, r2, 0);
        add(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

/* a test that does not halt at the end*/
void evilHaltTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 0);
        loadValue(sequence, r2, 0);
        add(sequence, r0, r1, r2);
        printOut(sequence, r0);
}
