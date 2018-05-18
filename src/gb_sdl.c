#include <stdio.h>
#include "gb_sdl.h"
#include "gb_lcd.h"

int create_SDL_window()
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		return 0;
	}

    //Set OpenGL version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    //Create window
    window = SDL_CreateWindow("OpenGL Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
                                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if(window == NULL)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    if((gl_context = SDL_GL_CreateContext(window)) == NULL) {
        fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return 0;
    }

    //Use Vsync
    if( SDL_GL_SetSwapInterval( 1 ) < 0 ) {
        printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
    }

    //Initialize OpenGL
    if(!init_GL())
    {
        fprintf(stderr, "Unable to initialize OpenGL!\n" );
        return 0;
    }

    
    /*//Get window surface
    screenSurface = SDL_GetWindowSurface( window );

    //Fill the surface white
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ));
    
    //Update the surface
    SDL_UpdateWindowSurface( window );*/

    //Wait two seconds
    SDL_Delay( 2000 );

	return 1;
}

int init_GL()
{
    //glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, -1.0, 1.0);
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DITHER);
	glDisable(GL_BLEND);
    return 1;
}

void render_game()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 	glLoadIdentity();
 	glRasterPos2i(-1, 1);
	glPixelZoom(1, -1);
 	glDrawPixels(160, 144, GL_RGB, GL_UNSIGNED_BYTE, screen_data);
	SDL_GL_SwapWindow(window);
}

void close1() 
{
    SDL_FreeSurface(screenSurface);
    screenSurface = NULL;
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();
}
