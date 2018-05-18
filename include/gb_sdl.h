#ifndef GB_SDL_H
#define GB_SDL_H

#include "gb_structs.h"
#include "SDL.h"
#include "SDL_opengl.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

//The window we'll be rendering to
SDL_Window* window;
    
//The surface contained by the window
SDL_Surface* screenSurface;

SDL_GLContext gl_context;

int create_SDL_window();

int init_GL();

void render_game();

void init_screen();

void close1();

#endif