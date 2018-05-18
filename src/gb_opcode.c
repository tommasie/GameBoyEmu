#include "gb_opcode.h"
#include "gb_memory.h"

BYTE NOOP() {
    return 4;
}

BYTE CPU_8BIT_LOAD(BYTE reg)
{
    BYTE data = read_memory(pc);
    pc++;
    reg = data;
    return 8;
}

BYTE CPU_REG_LOAD(BYTE reg_to, BYTE reg_from, BYTE cycles)
{
    reg_to = reg_from;
    return cycles;
}

BYTE CPU_REG_LOAD_ADDR(BYTE reg, WORD address)
{
    reg = read_memory(address);
    return 8;
}

BYTE CPU_ADDR_LOAD_REG(WORD address, BYTE reg)
{
    write_memory(address, reg);
    return 8;
}

BYTE CPU_8_BIT_ADDR_LOAD(WORD address)
{
    write_memory(address, read_memory(pc));
    pc++;
    return 12;
}

BYTE CPU_16BIT_LOAD(WORD reg)
{
    WORD data = read_memory(pc++);
    data <<= 8;
    data |= read_memory(pc++);
    reg = data;
    return 12;
}

BYTE CPU_PUSH(WORD reg)
{
    BYTE lo = reg >> 8;
    write_memory(sp.reg, lo);
    sp.reg--;
    BYTE hi = reg & 0xFF;
    write_memory(sp.reg, hi);
    sp.reg--;
    return 16;
}

BYTE CPU_POP(WORD* reg)
{
    sp.reg++;
    BYTE hi = read_memory(sp.reg);
    sp.reg++;
    BYTE lo = read_memory(sp.reg);
    WORD r = lo << 8;
    r |= hi;
    *reg = r;
    return 16;
}