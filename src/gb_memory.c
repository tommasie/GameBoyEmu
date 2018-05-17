#include "gb_memory.h"
#include "gb_timer.h"
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
        main_rom[address] = 0;
    }

    //Game is trying to change the value of the frequency
    else if(address == TAC) {
        BYTE current_freq = get_clock_frequency();
        cartridge_memory[address] = data;
        BYTE new_freq = get_clock_frequency();
        if(current_freq != new_freq) {
            set_clock_frequency();
        }
    }

    //Reset the current scanline if the game tries to write to it
    else if (address == 0xFF44) {
        main_rom[address] = 0 ;
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