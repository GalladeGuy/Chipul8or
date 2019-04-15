#include <string.h>
#include "main.h"
#include "input.h"

void mapKeys(int* array){
	memcpy(keys, array, 16 * sizeof(int));
}

void updateKeys(){
	SDL_PumpEvents();
}

int isKeyDown(BYTE key){
	return SDL_GetKeyboardState(NULL)[keys[key]];
}