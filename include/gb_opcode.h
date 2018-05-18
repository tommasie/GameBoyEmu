#ifndef GB_OPCODES_H
#define GB_OPCODES_H

#include "gb_structs.h"

//Include all the available OPCODES of the GB
//All OPCODES should return the number of cycles
//it takes them to execute according to the datasheet

BYTE NOOP();
BYTE CPU_8BIT_LOAD(BYTE reg);
BYTE CPU_REG_LOAD(BYTE reg, BYTE load, BYTE cycles);
BYTE CPU_REG_LOAD_ADDR(BYTE reg, WORD address);
BYTE CPU_ADDR_LOAD_REG(WORD address, BYTE reg);
BYTE CPU_8_BIT_ADDR_LOAD(WORD address);

BYTE CPU_16BIT_LOAD(WORD reg);

BYTE CPU_PUSH(WORD reg);
BYTE CPU_POP(WORD* reg);


#endif