#include <stdio.h>
#include "gb_emulator.h"
#include "gb_sdl.h"
int main()
{
    power_up();
    open_game();
    print_cartridge_data();

    create_SDL_window();
    render_game();
    close1();
    return 0;
}
