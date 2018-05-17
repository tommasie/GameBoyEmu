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
}

void write_memory(WORD address, BYTE data) {
    //read only memory, do not write
    if(address < 0x8000) {}

    //ECHO memory area, must be written in RAM area [C000-DDFF]
    else if(address >= 0xE000 && address < 0xFE00) {
        main_rom[address] = data;
        write_memory(address - 0x2000, data);
    }

    //[FEA0-FEFF] Not Usable
    else if(address >= 0xFEA0 && address < 0xFF00) {}

    //Otherwise write
    else {
        main_rom[address] = data;
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
