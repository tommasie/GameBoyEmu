#include "gameboy.h"
#include <string.h>

void init()
{
    //Program counter and stack pointer
    pc = 0x100;
    sp = (Register) { 0xFFFE };
    //Registers
    regAF = (Register) { 0x01B0 };
    regBC = (Register) { 0x0013 };
    regDE = (Register) { 0x00D8 };
    regHL = (Register) { 0x014D };
    //Internal ROM init
    //Set all values to 0
    memset(main_rom,0,sizeof(main_rom));
    //Set specific data according to datasheet
    main_rom[0xFF05] = 0x00;
    main_rom[0xFF06] = 0x00;
    main_rom[0xFF07] = 0x00;
    main_rom[0xFF10] = 0x80;
    main_rom[0xFF11] = 0xBF;
    main_rom[0xFF12] = 0xF3;
    main_rom[0xFF14] = 0xBF;
    main_rom[0xFF16] = 0x3F;
    main_rom[0xFF17] = 0x00;
    main_rom[0xFF19] = 0xBF;
    main_rom[0xFF1A] = 0x7F;
    main_rom[0xFF1B] = 0xFF;
    main_rom[0xFF1C] = 0x9F;
    main_rom[0xFF1E] = 0xBF;
    main_rom[0xFF20] = 0xFF;
    main_rom[0xFF21] = 0x00;
    main_rom[0xFF22] = 0x00;
    main_rom[0xFF23] = 0xBF;
    main_rom[0xFF24] = 0x77;
    main_rom[0xFF25] = 0xF3;
    main_rom[0xFF26] = 0xF1; //can be F0 for SGB
    main_rom[0xFF40] = 0x91;
    main_rom[0xFF42] = 0x00;
    main_rom[0xFF43] = 0x00;
    main_rom[0xFF45] = 0x00;
    main_rom[0xFF47] = 0xFC;
    main_rom[0xFF48] = 0xFF;
    main_rom[0xFF49] = 0xFF;
    main_rom[0xFF4A] = 0x00;
    main_rom[0xFF4B] = 0x00;
    main_rom[0xFFFF] = 0x00;

    //Initialize ROM banking
    MBC1 = 0;
    MBC2 = 0;
    current_rom_bank = 1;

    //Initialize RAM banking
    memset(ram_banks,0,sizeof(ram_banks));
    current_ram_bank = 0;
    ram_enabled = 0;

    //Timers
    divider_counter = 0;
}

void write_memory(WORD address, BYTE data) {
    //If program tries to write into the read only area [0x0,0x8000]
    //then banking is happening
    if(address < 0x8000) {
        handle_banking(address, data);
    }

    else if(address >= 0xA000 && address < 0xC000) {
        if(ram_enabled) {
            WORD new_address = address - 0xA000 ;
            ram_banks[new_address + (current_ram_bank*0x2000)] = data;
        }
    }

    //ECHO memory area, must be written in RAM area [C000-DDFF]
    else if(address >= 0xE000 && address < 0xFE00) {
        main_rom[address] = data;
        write_memory(address - 0x2000, data);
    }

    //[FEA0-FEFF] Not Usable
    else if(address >= 0xFEA0 && address < 0xFF00) {}

    else if(address == DIV) {
        main_rom[DIV] = 0;
    }

    //Game is trying to change the value of the frequency
    else if(address == TMC) {
        BYTE current_freq = get_clock_frequency();
        cartridge_memory[TMC] = data;
        BYTE new_freq = get_clock_frequency();
        if(current_freq != new_freq) {
            set_clock_frequency();
        }
    }

    //Otherwise write
    else {
        main_rom[address] = data;
    }
}

void handle_banking(WORD address, BYTE data)
{
    if(address < 0x2000) {
        //Enable RAM if address has 0x0A in the final 4 bits, otherwise disable it
        BYTE test = data & 0x0F;
        if(test == 0x0A) {
            ram_enabled = 1;
        }
        else if(test == 0x00) {
            ram_enabled = 0;
        }
    }
    else if(address < 0x4000) {
        //Select the lowest 5 bits of the new bank
        BYTE lo5 = data & 0x1F;
        //Zero the last 5 bits of the current bank variable
        current_rom_bank &= 0xFFE0;
        //Set the last 5 bits of the current_bank
        current_rom_bank |= lo5;
        //If the resulting bank is 0 (already included in memory), set 1
        if(current_rom_bank == 0) {
            current_rom_bank++;
        }
    }
    else if(address < 0x6000) {
        //Input is a 2-bit register
        if(banking_mode == ROM_BANKING_MODE) {
            //Set bits 5 and 6 of the ROM bank number
            data = data << 5;
            current_rom_bank &= 0xFF9F;
            current_rom_bank |= data;
            if(current_rom_bank == 0) {
                current_rom_bank++;
            }
        }
        else if(banking_mode == RAM_BANKING_MODE) {
            current_ram_bank = data & 0x03;
        }
    }
    else { //ROM/RAM mode select
        banking_mode = data & 0x01;
        if(banking_mode == ROM_BANKING_MODE)
            current_rom_bank = 0;
    }
}

BYTE read_memory(WORD address)
{
    //If reading from the ROM extra memory bank, do this
    if(address >= 0x4000 && address < 0x8000) {
        WORD new_address = address - 0x4000;
        return cartridge_memory[new_address + (current_rom_bank*0x4000)]; //each ROM bank is 0x4000 bytes in size
    }

    //If reading from RAM memory bank
    else if(address >= 0xA000 && address < 0xC000) {
        WORD new_address = address - 0xA000 ;
        return ram_banks[new_address + (current_ram_bank*0x2000)];
    }

    else {
        return main_rom[address];
    }
}

void open_game()
{
    //Read binary data and load it into a contiguous block of memory big enough to fit it
    memset(cartridge_memory, 0, sizeof(cartridge_memory));
    FILE* in = fopen("games/tetris.gb","rb");
    fread(cartridge_memory,sizeof(BYTE), sizeof(cartridge_memory), in);
    fclose(in);
}

void check_game_banking_mode()
{
    //ROM
    switch(cartridge_memory[0x0147]) {
        //NOTE: '...' in a switch to define a range-based case is a GCC extension, not standard C
        case 1 ... 3: MBC1 = 1; break;
        case 4 ... 6: MBC2 = 1; break;
        default: break;
    }

    //RAM
    current_ram_bank = cartridge_memory[0x0148];
}

BYTE clock_enabled()
{
    return read_memory(TMC) & 0b0100 != 0 ? 1 : 0;
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
    return read_memory(TMC) & 0b11;
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

