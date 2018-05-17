#include "gameboy.h"
#include <string.h>
#include "gb_memory.h"
#include "gb_timer.h"
#include "gb_interrupt.h"

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
    
    //Interrupts
    IME = 1;
}

void open_game()
{
    //Read binary data and load it into a contiguous block of memory big enough to fit it
    memset(cartridge_memory, 0, sizeof(cartridge_memory));
    FILE* in = fopen("../games/tetris.gb","rb");
    if(in == NULL) {
        fprintf(stderr, "Errore lettura del file\n");
        return;
    }
    fread(cartridge_memory,sizeof(BYTE), sizeof(cartridge_memory), in);
    fclose(in);
}

void show_nintendo_logo()
{
    int i;
    for(i = 0x0104; i < 0x0134; i++) {
        printf("%02x ", cartridge_memory[i]);
    }
    printf("\n");
}
