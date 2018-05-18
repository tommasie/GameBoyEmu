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
    BYTE control = read_memory(LCDC);
    if(control & 0x01) {
        render_tiles();
    }
    if(control & 0x02) {
        render_sprites();
    }
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

void dma_transfer(BYTE data)
{
    WORD address = data << 8 ; // source address is data * 100
    int i;
    for (i = 0 ; i < 0xA0; i++) {
        write_memory(0xFE00 + i, read_memory(address + i)) ;
    }
}

void render_tiles()
{
    BYTE control = read_memory(LCDC);
    WORD tile_data = 0;
    WORD bg_memory = 0; 
    BYTE unsig = 1;

    // where to draw the visual area and the window
    BYTE scroll_y = read_memory(SCY) ;
    BYTE scroll_x = read_memory(SCX) ;
    BYTE window_y = read_memory(WY) ;
    BYTE window_x = read_memory(WX) - 7; 

    BYTE using_window = 0;

    //Check if Window Display is enabled (bit 5 of LCDC)
    if(control & 0x20) {
        //Check if scanline is within WY pos
        if(window_y <= read_memory(CURR_SCANLINE_ADDR)) {
            using_window = 1;
        }
    }

    //Check used tile data (bit 4 of LCDC)
    if(control & 0x10) {
        tile_data = 0x8000;
    }
    else {
        tile_data = 0x8800;
        //Bytes are signed, for whatever reason
        unsig = 0;
    }

    if(!using_window) {
        BYTE bg_region = control & 0x08;
        if(bg_region) {
            //region = [9C00-9FFF]
            bg_memory = 0x9C00;
        }
        else {
            //region = [9800-9BFF]
            bg_memory = 0x9800;
        }
    }
    else {
        BYTE window_region = control & 0x40;
        if(window_region) {
            bg_memory = 0x9C00;
        }
        else {
            bg_memory = 0x9800;
        }
    }

    BYTE yPos = 0 ; 

    // yPos is used to calculate which of 32 vertical tiles the 
    // current scanline is drawing
    if(!using_window)
        yPos = scroll_y + read_memory(CURR_SCANLINE_ADDR);
    else
        yPos = read_memory(CURR_SCANLINE_ADDR) - window_y;
    
    WORD tileRow = (((BYTE)(yPos/8))*32);

    //Drae the 160 horizontal pixels for the current scanline
    for (int pixel = 0 ; pixel < 160; pixel++) {
        BYTE xPos = pixel + scroll_x;

        // translate the current x pos to window space if necessary
        if (using_window) {
            if (pixel >= window_x) {
                xPos = pixel - window_x;
            }
        } 

        // which of the 32 horizontal tiles does this xPos fall within?
        WORD tileCol = (xPos/8); 
        SIGNED_WORD tileNum;

        // get the tile identity number. Remember it can be signed
        // or unsigned
        WORD tileAddrss = bg_memory + tileRow + tileCol;
        if(unsig)
            tileNum =(BYTE)read_memory(tileAddrss);
        else
            tileNum =(SIGNED_BYTE)read_memory(tileAddrss );

        // deduce where this tile identifier is in memory. Remember i 
        // shown this algorithm earlier
        WORD tileLocation = tile_data ;

        if (unsig)
            tileLocation += (tileNum * 16);
        else
            tileLocation += ((tileNum + 128) *16);

        // find the correct vertical line we're on of the 
        // tile to get the tile data 
        //from in memory
        BYTE line = yPos % 8 ;
        line *= 2; // each vertical line takes up two bytes of memory
        BYTE data1 = read_memory(tileLocation + line); 
        BYTE data2 = read_memory(tileLocation + line + 1);

        // pixel 0 in the tile is it 7 of data 1 and data2.
        // Pixel 1 is bit 6 etc..
        int colourBit = xPos % 8;
        colourBit -= 7;
        colourBit *= -1;

        // combine data 2 and data 1 to get the colour id for this pixel 
        // in the tile
        int colourNum = BitGetVal(data2, colourBit);
        colourNum <<= 1;
        colourNum |= BitGetVal(data1,colourBit);

        // now we have the colour id get the actual 
        // colour from palette 0xFF47
        COLOUR col = get_colour(colourNum, 0xFF47) ;
        int red = 0;
        int green = 0;
        int blue = 0;

        // setup the RGB values
        switch(col) {
            case WHITE:	red = 255; green = 255; blue = 255; break;
            case LIGHT_GRAY: red = 0xCC; green = 0xCC; blue = 0xCC; break;
            case DARK_GRAY:	red = 0x77; green = 0x77; blue = 0x77; break;
        }

        int current_line = read_memory(CURR_SCANLINE_ADDR) ;

        // safety check to make sure what im about 
        // to set is int the 160x144 bounds
        if ((current_line < 0) || (current_line > 143) || (pixel < 0)|| (pixel > 159)) {
            continue;
        }

        screen_data[pixel][current_line][0] = red;
        screen_data[pixel][current_line][1] = green;
        screen_data[pixel][current_line][2] = blue;
    }
    
}

void render_sprites()
{
    BYTE control = read_memory(LCDC);
    if (control & 0x02) {
		BYTE use8x16 = 0;
		if (control & 0x04)
			use8x16 = 1;

        int sprite;
		for (sprite = 0 ; sprite < 40; sprite++) {
 			BYTE index = sprite * 4 ;
 			BYTE yPos = read_memory(0xFE00 + index) - 16;
 			BYTE xPos = read_memory(0xFE00 + index + 1) - 8;
 			BYTE tileLocation = read_memory(0xFE00 + index + 2) ;
 			BYTE attributes = read_memory(0xFE00 + index + 3) ;

			int yFlip = attributes & 0x40;
			int xFlip = attributes & 0x20;

			int scanline = read_memory(CURR_SCANLINE_ADDR);

			int ysize = 8;

			if(use8x16)
				ysize = 16;

 			if((scanline >= yPos) && (scanline < (yPos + ysize))) {
 				int line = scanline - yPos;

 				if (yFlip) {
 					line -= ysize;
 					line *= -1;
 				}

 				line *= 2;
 				BYTE data1 = read_memory( (0x8000 + (tileLocation * 16)) + line);
 				BYTE data2 = read_memory( (0x8000 + (tileLocation * 16)) + line+1);

                int tilePixel;
 				for (tilePixel = 7; tilePixel >= 0; tilePixel--) {
					int colourbit = tilePixel;
 					if (xFlip) {
 						colourbit -= 7 ;
 						colourbit *= -1 ;
 					}
 					int colourNum = BitGetVal(data2,colourbit);
 					colourNum <<= 1;
 					colourNum |= BitGetVal(data1,colourbit);

					COLOUR col = get_colour(colourNum, attributes & 0x10 ? 0xFF49 : 0xFF48);

 					// white is transparent for sprites.
 					if (col == WHITE)
 						continue;

 					int red = 0;
 					int green = 0;
 					int blue = 0;

					switch(col){
                        case WHITE:	red = 255; green = 255; blue = 255; break;
                        case LIGHT_GRAY:red = 0xCC; green = 0xCC; blue = 0xCC; break;
                        case DARK_GRAY:	red = 0x77; green = 0x77; blue = 0x77; break;
					}

 					int xPix = 0 - tilePixel ;
 					xPix += 7 ;

					int pixel = xPos+xPix ;

					if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
					{
					//	assert(false) ;
						continue;
					}

					// check if pixel is hidden behind background
					if (attributes && 0x80)
					{
						if ( (screen_data[scanline][pixel][0] != 255) || (screen_data[scanline][pixel][1] != 255) || (screen_data[scanline][pixel][2] != 255) )
							continue ;
					}

 					screen_data[scanline][pixel][0] = red ;
 					screen_data[scanline][pixel][1] = green ;
 					screen_data[scanline][pixel][2] = blue ;

 				}
 			}
		}
	}
}

COLOUR get_colour(BYTE colour_num, WORD address)
{
    COLOUR res = WHITE;
    BYTE palette = read_memory(address);
    int hi = 0;
    int lo = 0;

    // which bits of the colour palette does the colour id map to?
    switch (colour_num) 
    {
        case 0: hi = 1; lo = 0; break;
        case 1: hi = 3; lo = 2; break; 
        case 2: hi = 5; lo = 4; break; 
        case 3: hi = 7; lo = 6; break;
    }

    // use the palette to get the colour
    int colour = 0;
    colour = BitGetVal(palette, hi) << 1;
    colour |= BitGetVal(palette, lo) ;

    // convert the game colour to emulator colour
    switch (colour)
    {
        case 0: res = WHITE; break;
        case 1: res = LIGHT_GRAY; break;
        case 2: res = DARK_GRAY; break;
        case 3: res = BLACK; break;
    }

    return res;
}

int BitGetVal(BYTE data, int position) {
    int mask = 1 << position ;
	return ( data & mask ) ? 1 : 0 ;
}