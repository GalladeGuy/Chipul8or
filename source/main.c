#include <stdio.h>
#include "main.h"
#include "cpu.h"
#include "gfx.h"
#include "memory.h"
#include "input.h"


int main(int argc, char** argv){
	SDL_SetMainReady();
	SDL_Init(SDL_INIT_EVERYTHING);
	
	initWindow(16);
	initMemory();
	initCPU();
	mapKeys(defaultKeys);
	
	if(argc == 1){
		printf("Please open this emulator with a game!\nPress any key to exit!\n");
		getchar();
		term();
		return 0;
	}
	
	loadROM(argv[1]);
	
	displayGraphics(); //Just so there's no white screen at the start
	
	//dumpMemory();
	
	int isRunning = 1;
	printf("Initialization complete\n");
	
	while(isRunning){
		loadOp();
		//printRegisters();
		execOp();
		//getchar();
		SDL_Delay(1);
	}
	
	getchar();
	term();
	return 0;
}

void term(){
	termMemory();
	termCPU();
	termWindow();
	
	SDL_Quit();
}