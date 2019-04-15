#include "main.h"
#include "gfx.h"

void initWindow(int scale){
	gfx = malloc(sizeof(GFX));
	gfx->w = SDL_CreateWindow("Chipul8or", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 64 * scale, 32 * scale, SDL_WINDOW_SHOWN);
	gfx->r = SDL_CreateRenderer(gfx->w, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(gfx->r, 0, 0, 0, 0xFF);
	gfx->t = SDL_CreateTexture(gfx->r, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	for(int i = 0; i < 2048; i++){
		gfx->fbr[i] = 0;
	}
}

void termWindow(){
	SDL_DestroyWindow(gfx->w);
	
	free(gfx);
	gfx = NULL;
}

void displayGraphics(){
	SDL_RenderClear(gfx->r);
	SDL_RenderCopy(gfx->r, gfx->t, NULL, NULL);
	SDL_RenderPresent(gfx->r);
}