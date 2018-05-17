#include "gb_lcd.h"
#include "gb_memory.h"
#include "gb_interrupt.h"

BYTE lcd_enabled()
{
    return read_memory(LCDC) & 0x80;
}

void set_lcd_status() 
{
    BYTE status = read_memory(LCD_STATUS);
    //If the LCD is not enabled, LCD_STATUS has to be set to Mode 1 (V-Blank)
    //and the scanline reset to 0 and the counter to 456
    if(!lcd_enabled()) {
        scanline_counter = 456;
        main_rom[CURR_SCANLINE_ADDR] = 0;
        status |= 0x1;
        status &= 0xFD;
        write_memory(LCD_STATUS, status);
        return;
    }

    BYTE current_line = read_memory(CURR_SCANLINE_ADDR); 
    BYTE current_mode = status & 0x3;
    BYTE mode = 0;
    BYTE req_int = 0;

    if(current_line >= 144) {
        mode = 1;
        status |= 0x1;
        status &= 0xFD;
        req_int = status & 0x10;
    }
    else {
        int mode2bounds = 456-80;
        int mode3bounds = mode2bounds - 172; 

        // mode 2
        if (scanline_counter >= mode2bounds) { 
            mode = 2;
            status |= 0x10;
            status &= 0xFE;
            req_int = status & 0x20;
        }
        // mode 3
        else if(scanline_counter >= mode3bounds) {
            mode = 3;
            status |= 0x11;
        }
        // mode 0
        else {
            mode = 0;
            status &= 0xFC;
            req_int = status & 0x08 ; 
        }
    }

    // just entered a new mode so request interrupt
    if (req_int && (mode != current_mode))
        request_interrupt(1) ;

    // check the conincidence flag
    if (current_line == read_memory(LYC))
    {
        status |= 1 << 2;
        if(status & 0x40) {
            request_interrupt(1);
        }
    }
    else {
        //Set bit 2 to zero
        status &= ~(1 << 2);
    }
    write_memory(LCD_STATUS,status); 

}

void draw_scan_line()
{

}

void update_graphics(int cycles)
{
    set_lcd_status();
    if(lcd_enabled) {
        scanline_counter -= cycles;
    }
    else return;

    if(scanline_counter <= 0) {
        //increment scanline register
        main_rom[CURR_SCANLINE_ADDR]++;
        BYTE current_line = read_memory(CURR_SCANLINE_ADDR);

        scanline_counter = 456;

        //Begin vertical blank period, request V-Blank interrupt
        if(current_line >= 144 && current_line < 153) {
            request_interrupt(0);
        }
        //Last scanline available, reset counter to zero
        else if(current_line > 153) {
            main_rom[CURR_SCANLINE_ADDR] = 0;
        }
        else if (current_line < 144) {
            draw_scan_line();
        }
    }

}