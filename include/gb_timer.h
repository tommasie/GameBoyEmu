#include "gameboy.h"
//CPU clock frequency (4.194304MHz)
#define CLOCKSPEED 4194304
/** Registers **/
//Divider Register, incremented at a rate of 16384Hz. Any other write sets it to zero
#define DIV 0xFF04
//Timer counter, incremented at afrecequcy specified by TAC. When it overflows, it's reset to $TMA
#define TIMA 0xFF05
//Timer modulo, when the TIMA overflows, this data will be loaded.
#define TMA 0xFF06
//Timer control, bit 2 {0=stop, 1=start}, bits 1-0 input clock select (4 different frequencies, see datasheet)
#define TAC 0xFF07

int timer_counter;
int divider_counter;

BYTE clock_enabled();

void update_timers(int cycles);

void increase_divider_register(int cycles);
BYTE get_clock_frequency();
void set_clock_frequency();