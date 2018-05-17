#include "gb_timer.h"
#include "gb_memory.h"

BYTE clock_enabled()
{
    return read_memory(TAC) & 0b0100 != 0 ? 1 : 0;
}

void update_timers(int cycles)
{
    increase_divider_register(cycles);

    if(clock_enabled()) {
        timer_counter -= cycles;
        //Update the timer
        if(timer_counter < 0) {
            set_clock_frequency();
            //Manage the overflow (8-bit register)
            if(read_memory(TIMA) == 255) {
                //Reset TIMA value with the one in TMA
                write_memory(TIMA, read_memory(TMA));
                //request_interrupt(2);
            } else {
                write_memory(TIMA, read_memory(TIMA + 1));
            }
        }
    }
}

void increase_divider_register(int cycles)
{
    divider_counter += cycles;
    if(divider_counter >= 255) {
        divider_counter = 0;
        main_rom[DIV]++;
    }
}

BYTE get_clock_frequency()
{
    return read_memory(TAC) & 0b11;
}

void set_clock_frequency()
{
    BYTE freq = get_clock_frequency();
    switch(freq) {
        case 0: timer_counter = CLOCKSPEED / 4096; break;
        case 1: timer_counter = CLOCKSPEED / 262144; break;
        case 2: timer_counter = CLOCKSPEED / 65536; break;
        case 3: timer_counter = CLOCKSPEED / 16384; break;
    }
}