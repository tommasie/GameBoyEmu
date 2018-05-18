#include "gameboy.h"

//LCD Control
#define LCDC 0xFF40

//Address containing the current LCD status, 4 bits [0x00, 0x11]
//00 : H-Blank period
//01 : V-Blank period
//10 : Searching OAM-RAM (OAM = Sprite Attribute Table)
//11 : Transfering Data to LCD Driver
#define LCD_STATUS 0xFF41

/** Position and Scrolling **/
//Specifies the position in the 256x256 pixels BG map (32x32 tiles) 
//which is to be displayed at the upper/left LCD display position.
//Scroll Y
#define SCY 0xFF42
//Scroll X
#define SCX 0xFF43
//Specifies the upper/left positions of the Window area.
//Window Y Position
#define WY 0xFF4A
//Window X Position minus 7
#define WX 0xFF4B

//Address containing the current scanline value [0,153]
#define CURR_SCANLINE_ADDR 0xFF44

#define LYC 0xFF45

//Direct Memory Access
#define DMA 0xFF46

//Screen (size 160X144 px, each pixel is RGB)
BYTE screen_data[160][144][3];

//456 clock cycles are needed to draw a scanline and move to next
int scanline_counter;

typedef enum COLOUR
{
    WHITE,
    LIGHT_GRAY,
    DARK_GRAY,
    BLACK
} COLOUR;

BYTE lcd_enabled();
void set_lcd_status();
void draw_scan_line();
void update_graphics(int cycles);
void dma_transfer(BYTE data);

void render_tiles();
void render_sprites();

COLOUR get_colour(BYTE colour_num, WORD address);
int BitGetVal(BYTE data, int position);