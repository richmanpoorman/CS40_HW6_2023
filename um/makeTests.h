#include <stdint.h>
#include <seq.h>


typedef enum REGISTER {
        r0 = 0,
        r1 = 1,
        r2 = 2,
        r3 = 3,
        r4 = 4,
        r5 = 5,
        r6 = 6,
        r7 = 7
} REGISTER;

typedef uint32_t Instruction;

void cmov(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc);
void cmove(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc);
void segLoad(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc);
void segStore(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc);
void add(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc);
void mult(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc);
void divide(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc);
void nand(Seq_T stream, REGISTER ra, REGISTER rb, REGISTER rc);
void halt(Seq_T stream);
void mapSeg(Seq_T stream, REGISTER rb, REGISTER rc);
void unmapSeg(Seq_T stream, REGISTER rc);
void printOut(Seq_T stream, REGISTER rc);
void readIn(Seq_T stream, REGISTER rc);
void loadProgram(Seq_T stream, REGISTER rb, REGISTER rc);
void loadValue(Seq_T stream, REGISTER rl, REGISTER value);

void Um_write_sequence(FILE *output, Seq_T instructions);