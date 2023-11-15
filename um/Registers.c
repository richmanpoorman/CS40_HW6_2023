/* 
 *   Name       : Registers.c
 *   Assignment : CS40 Homework 6 (um)
 *   Purpose    : Module representing the registers of our Universal Machine
 *   Editors    : Matthew Wong (mwong14), Ivi Fung (sfung02)
 */

#include <assert.h>
#include <mem.h>
#include <stdlib.h>

typedef struct Registers {
    uint_32 registerArray[8];
} *Registers;

/*
 *  Name      : Registers_new
 *  Purpose   : Creates a new set of registers
 *  Parameters: (None)
 *  Returns   : (None)
 *  Notes     : Will CRE on failure to allocate memory
 *              
 */
void Registers_new() 
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
 *              (uint_32) regID -- Position/ID of the register to access
 *              (uint_32) data -- Data to store in the register
 *  Returns   : (None)
 *  Notes     : Will CRE if the pointer Registers is null;
 *              Will CRE if the data in the pointer is null;
 */
void Registers_setRegisters(Registers reg, uint_32 regID, uint_32 data)
{
        assert(reg != NULL && *reg != NULL);
        reg->registerArray[regID] = data;
}

/*
 *  Name      : Registers_getRegisters
 *  Purpose   : Gets the data stored in given register within the set
 *  Parameters: (Registers) Set of registers
 *              (uint_32) reg -- Position/ID of the register to access
 *  Output    : (uint_32) regID -- Data stored in the accessed registers
 *  Notes     : Will CRE if the pointer Registers is null;
 *              Will CRE if the data in the pointer is null;      
 */
void Registers_getRegisters(Registers reg, uint_32 regID)
{
        assert(regPointer != NULL && *regPointer != NULL);
        return reg->registerArray[regId];
}
