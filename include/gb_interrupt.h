#include "gameboy.h"

/** Interrupts **/
//Interrupt Enable
#define IE 0xFFFF
//Interrupt Request
#define IF 0xFF0F
//Interrupt Master Enable (boolean flag)
BYTE IME;

void request_interrupt(int id);
void exec_interrupts();
void serve_interrupt(int interrupt);