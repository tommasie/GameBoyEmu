#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <stdio.h>
#include <stdlib.h>

//Basic data types
typedef unsigned char BYTE;
typedef char SIGNED_BYTE;
typedef unsigned short WORD; 
typedef signed short SIGNED_WORD;

//Registers
//8 8-bit registers A,B,C,D,E,F,H,L
//which are usually combined together to get a 16-bit register
//AF,BC,DE,HL
typedef union Register
{
    WORD reg;
    //lo before hi because big-endian CPU
    struct
    {
        BYTE lo;
        BYTE hi;
    };
} Register;

Register regAF;
Register regBC;
Register regDE;
Register regHL;

//Bit flags register (F)
//Each value is the bit number
#define FLAG_Z 7; //zero flag: This bit becomes set (1) if the result of an operation has been zero (0). Used for conditional JMP.
#define FLAG_N 6; //add/sub flag: N Indicates whether the previous instruction has been an addition or subtraction (only used with DAA)
#define FLAG_H 5; //half carry flag: , H indicates carry for lower 4bits of the result (only used with DAA)
#define FLAG_C 4; //carry flag: Becomes set when the result of an addition became bigger than FFh (8bit) or FFFFh (16bit)

//Gameboy ROM memory (range from 0x0 to 0xFFFF)
BYTE main_rom[0x10000];

//Program counter (initialized to 0x100)
WORD pc;

//Stack pointer (initialized to 0xFFFE)
Register sp;

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

//Timers
#define DIV 0xFF04
#define TIMA 0xFF05
#define TMA 0xFF06
#define TMC 0xFF07

#define CLOCKSPEED 4194304
int timer_counter;
int divider_counter;

//Interrupts
#define IE 0xFFFF
#define IF 0xFF0F

//Screen (size 160X144 px, each pixel is RGB)
BYTE screen_data[160][144][3];

//Functions
//Power-up sequence
void init();

void write_memory(WORD address, BYTE data);

void handle_banking(WORD address, BYTE data);

BYTE read_memory(WORD address);

void open_game();

void check_game_banking_mode();

BYTE clock_enabled();

void update_timers(int cycles);

void increase_divider_register(int cycles);
BYTE get_clock_frequency();
void set_clock_frequency();

#endif