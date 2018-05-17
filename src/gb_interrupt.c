#include "gb_interrupt.h"
#include "gb_memory.h"

void request_interrupt(int id)
{
    BYTE req = read_memory(IF);
    req |= 0b1 << id;
    write_memory(IF,req);
}

void exec_interrupts()
{
    if(IME) {
        BYTE req = read_memory(IF);
        BYTE enabled = read_memory(IE);
        if(req > 0) {
            int i;
            for(i = 0; i < 5; i++) {
                if(req & (0b1 << i)) {
                    if(enabled & (0b1 << i)) {
                        serve_interrupt(i);
                    }
                }

            }
        }
    }
}

void serve_interrupt(int interrupt)
{
    //Disable interrupts
    IME = 0;
    //Set requested IF bit to zero
    BYTE req = read_memory(IF);
    req ^= 0b1 << interrupt;
    write_memory(IF,req);

    //Push the current execution address on the stack
    //push_on_stack(pc);

    //Move control to the interrupt code
    switch(interrupt) {
        //V-Blank
        case 0: pc = 0x40; break;
        //LCD_STAT
        case 1: pc = 0x48; break;
        //Timer
        case 2: pc = 0x50; break;
        //Serial
        case 3: pc = 0x58; break;
        //Joypad
        case 4: pc = 0x60; break;
    }

}