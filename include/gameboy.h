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

//Program counter (initialized to 0x100)
WORD pc;

//Stack pointer (initialized to 0xFFFE)
Register sp;

//Functions
//Power-up sequence
void init();

void open_game();

void show_nintendo_logo();

#endif