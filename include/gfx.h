#ifndef GFX_H
#define GFX_H

#include "memory.h"

typedef struct{
	SDL_Window* w;		//Window
	SDL_Renderer* r;		//Renderer
	SDL_Texture* t;		//Screen Texture
	BYTE fbr[2048];		//Framebuffer
}GFX;

GFX* gfx;

void initWindow(int scale);
void termWindow();
void displayGraphics();

#endif