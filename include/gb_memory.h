#include "gameboy.h"

//Gameboy ROM memory (range from 0x0 to 0xFFFF)
BYTE main_rom[0x10000];

//Maximum cartridge size
BYTE cartridge_memory[0x200000];

//ROM BANKING -> extra memory included in the cartridge if the game's size exceeds 0x8000 bytes
//First bank is always loaded, 1..N banks are optional and loaded according to the game's state

//Banking type, deducted by cartridge memory block 0x0147
//Boolean flags
BYTE MBC1;
BYTE MBC2;
//Bank currently in use, should never be 0 since that is always loaded in main memory [0x0, 0x3FFF]
//Extra banks are loaded in [0x4000-0x7FFF]
BYTE current_rom_bank;

//RAM Banking, similar to ROM, tells the console wether the game is using extra RAM included in the cartridge
//Cartridges can have up to 4 banks of 0x2000 bytes each, carteidge memory block 0x0148 tells which bank is in use
BYTE ram_enabled;
BYTE ram_banks[0x8000];
BYTE current_ram_bank;

//Boolean value used to set RAM or ROM banking mode for write in range [0x4000,0x5FFF]
//Values: 0x00-> ROM; 0x01-> RAM.
#define ROM_BANKING_MODE 0x00
#define RAM_BANKING_MODE 0x01
BYTE banking_mode;

void write_memory(WORD address, BYTE data);

void handle_banking(WORD address, BYTE data);

BYTE read_memory(WORD address);

void check_game_banking_mode();