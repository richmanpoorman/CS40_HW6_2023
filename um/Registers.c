/* 
 *   Name       : Registers.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module representing the registers of our Universal Machine
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include <assert.h>
#include <mem.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct Registers {
    uint32_t registerArray[8];
} *Registers;

/*
 *  Name      : Registers_new
 *  Purpose   : Creates a new set of registers
 *  Parameters: (None)
 *  Returns   : (None)
 *  Notes     : Will CRE on failure to allocate memory
 *              
 */
Registers Registers_new() 
{
        Registers newRegisters = ALLOC(sizeof(*newRegisters));
        return newRegisters;
}

/*
 *  Name      : Registers_free
 *  Purpose   : Frees a set of registers
 *  Parameters: (Registers *) regPointer -- Pointer to the Registers to free
 *  Returns   : (None)
 *  Notes     : Will CRE if the pointer Registers is null;
 *              Will CRE if the data in the pointer is null;      
 */
void Registers_free(Registers *regPointer) 
{
        assert(regPointer != NULL && *regPointer != NULL);
        FREE(*regPointer);
        *regPointer = NULL;
}

/*
 *  Name      : Registers_setRegisters
 *  Purpose   : Sets a given register within the set
 *  Parameters: (Registers) reg -- Set of registers to update
 *              (uint32_t) regID -- Position/ID of the register to access
 *              (uint32_t) data -- Data to store in the register
 *  Returns   : (None)
 *  Notes     : Will CRE if the pointer Registers is null;
 *              Will CRE if the data in the pointer is null;
 */
void Registers_setRegisters(Registers reg, uint32_t regID, uint32_t data)
{
        assert(reg != NULL);
        reg->registerArray[regID] = data;
}

/*
 *  Name      : Registers_getRegisters
 *  Purpose   : Gets the data stored in given register within the set
 *  Parameters: (Registers) Set of registers
 *              (uint32_t) reg -- Position/ID of the register to access
 *  Output    : (uint32_t) regID -- Data stored in the accessed registers
 *  Notes     : Will CRE if the pointer Registers is null;
 *              Will CRE if the data in the pointer is null;      
 */
uint32_t Registers_getRegisters(Registers reg, uint32_t regID)
{
        assert(reg != NULL);
        return reg->registerArray[regID];
}
