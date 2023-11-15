/* 
 *   Name       : Registers.h
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module representing the registers of our Universal Machine
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

typedef struct Registers *Registers;

void Registers_new();
void Registers_free(Registers *reg);
void Registers_setRegisters(Registers reg, uint_32 regID, uint_32 data);
void Registers_getRegisters(Registers reg, uint_32 regID);


