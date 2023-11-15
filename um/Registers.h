/* 
 *   Name       : Registers.h
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module representing the registers of our Universal Machine
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */
#include <stdint.h>

typedef struct Registers *Registers;

void Registers_new();
void Registers_free(Registers *reg);
void Registers_setRegisters(Registers reg, uint32_t regID, uint32_t data);
void Registers_getRegisters(Registers reg, uint32_t regID);


