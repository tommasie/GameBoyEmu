#include "gameboy.h"

//Include all the available OPCODES of the GB
//All OPCODES should return the number of cycles
//it takes them to execute according to the datasheet

int NOOP();
int CPU_8BIT_LOAD(BYTE reg);
int CPU_REG_LOAD(BYTE reg, BYTE load, int cycles);
int CPU_REG_LOAD_ROM(BYTE reg, WORD address);