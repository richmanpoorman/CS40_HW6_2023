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