#include "makeTests.h"
#include <seq.h>

/* 
void TEST(Seq_T stream)
{

}
*/

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