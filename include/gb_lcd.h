#include "gameboy.h"

//LCD Control
#define LCDC 0xFF40
//Address containing the current LCD status, 4 bits [0x00, 0x11]
//00 : H-Blank period
//01 : V-Blank period
//10 : Searching OAM-RAM (OAM = Sprite Attribute Table)
//11 : Transfering Data to LCD Driver
#define LCD_STATUS 0xFF41
//Address containing the current scanline value [0,153]
#define CURR_SCANLINE_ADDR 0xFF44

#define LYC 0xFF45
//Screen (size 160X144 px, each pixel is RGB)
BYTE screen_data[160][144][3];

//456 clock cycles are needed to draw a scanline and move to next
int scanline_counter;

BYTE lcd_enabled();
void set_lcd_status();
void draw_scan_line();
void update_graphics(int cycles);