#include "gb_opcode.h"
#include "gb_memory.h"

int NOOP() {
    return 4;
}

int CPU_8BIT_LOAD(BYTE reg)
{
    BYTE data = read_memory(pc);
    pc++;
    reg = n;
    return 8;
}

int CPU_REG_LOAD(BYTE reg, BYTE load, int cycles)
{
    reg = load;
    return cycles;
}

int CPU_REG_LOAD_ROM(BYTE reg, WORD address)
{
    reg = read_memory(address);
    return 8;
}