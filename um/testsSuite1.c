#include "makeTests.h"
#include <seq.h>

/* void TEST(Seq_T sequence) 
{

}*/

void add_twoZerosTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 0);
        loadValue(sequence, r2, 0);
        add(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

void add_twoNonzerosTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 64);
        loadValue(sequence, r2, 35);
        add(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

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
//have not been added yet
void add_toSelfTest(Seq_T sequence) 
{
        loadValue(sequence, r0, 77);
        add(sequence, r0, r0, r0);
        printOut(sequence, r0);
        halt(sequence); 
}

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

void mult_twoZerosTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 0);
        loadValue(sequence, r2, 0);
        mult(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

void mult_twoNonzerosTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 36);
        loadValue(sequence, r2, 5);
        mult(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

void mult_threeNumsTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 90);
        loadValue(sequence, r2, 2);
        mult(sequence, r0, r1, r2);
        loadValue(sequence, r3, 23);
        mult(sequence, r4, r0, r3);
        printOut(sequence, r4);
        halt(sequence); 
}

void mult_toSelfTest(Seq_T sequence) 
{
        loadValue(sequence, r0, 5);
        mult(sequence, r0, r0, r0);
        printOut(sequence, r0);
        halt(sequence); 
}

void mult_overflowTest(Seq_T sequence) 
{
        uint32_t maxValue = 0;
        maxValue = ~maxValue;
        loadValue(sequence, r1, 30);
        loadValue(sequence, r2, maxValue - 1)
        mult(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

void div_evenIdentityTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 54);
        loadValue(sequence, r2, 1);
        divide(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

void div_oddIdentityTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 55);
        loadValue(sequence, r2, 1);
        divide(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

void div_evenByTwoTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 54);
        loadValue(sequence, r2, 2);
        divide(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

void div_oddByTwoTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 55);
        loadValue(sequence, r2, 2);
        divide(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

void div_bySelfTest(Seq_T sequence) 
{
        loadValue(sequence, r0, 105);
        divide(sequence, r0, r0, r0);
        printOut(sequence, r0);
        halt(sequence); 
}

/* no good naughty tests */
void div_zeroTest(Seq_T sequence) 
{
        loadValue(sequence, r1, 0);
        loadValue(sequence, r2, 0);
        add(sequence, r0, r1, r2);
        printOut(sequence, r0);
        halt(sequence); 
}

