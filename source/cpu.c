#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "cpu.h"

void initCPU(){
	cpu = malloc(sizeof(CPU));
	resetRegisters();
	srand(time(NULL));
}

void termCPU(){
	free(cpu);
	cpu = NULL;
}

void resetRegisters(){
	for(int i = 0; i < 0x10; i++){
		cpu->reg[i] = 0;
	}
	cpu->ir.W = 0;
	cpu->pc.W = 0x200;
	cpu->sp.W = 0;
	cpu->dt.counter = 0;
	SDL_RemoveTimer(cpu->dt.timer);
	cpu->dt.timer = SDL_AddTimer(16, timerDecrease, &cpu->dt);
	cpu->st.counter = 0;
	SDL_RemoveTimer(cpu->st.timer);
	cpu->st.timer = SDL_AddTimer(16, timerDecrease, &cpu->st);
	cpu->arg.W = 0;
	cpu->op.W = 0;
}

void badOp(){
	printf("Unrecognized Opcode: 0x%x\n", cpu->op.W);
	printRegisters();
	//getchar();
}

void loadOp(){
	cpu->op.W = __builtin_bswap16(((WORD*) mem)[cpu->pc.W >> 1].W);
}

void execOp(){
	cpu->arg.W = opArgs;
	switch(cpu->op.W >> 12){
		case 0x0:
			if(arg2b == 0xEE){
				RET();					//Return							-	00EE
			}
			else{
				CLS();					//Clear Screen					-	00E0
			}
			break;
		case 0x1:
			JMP();						//Jump 							-	1nnn
			break;
		case 0x2:
			CALL();					//Call								-	2nnn
			break;
		case 0x3:
			SEI();						//Skip If Equal Immediate		-	3xkk
			break;
		case 0x4:
			SNEI();					//Skip Not Equal Immediate	-	4xkk
			break;
		case 0x5:
			SER();						//Skip If Equal Register		-	5xy0
			break;
		case 0x6:
			LDI();						//Load Immediate				-	6xkk
			break;
		case 0x7:
			ADI();						//Add Immediate					-	7xkk
			break;
		case 0x8:
			switch(arg2n2){
				case 0x0: LDR();		//Load Register					-	8xy0
					break;
				case 0x1: OR();		//Bitwise OR						-	8xy1
					break;
				case 0x2: AND();		//Bitwise AND					-	8xy2
					break;
				case 0x3: XOR();		//Bitwise XOR					-	8xy3
					break;
				case 0x4: ADDR();	//Add Registers					-	8xy4
					break;
				case 0x5: SUB();		//Subtract (Vx - Vy)			-	8xy5
					break;
				case 0x6: SHR();		//Shift Right					-	8xy6
					break;
				case 0x7: SUBN();	//Subtract (Vy - Vx)			-	8xy7
					break;
				case 0xE: SHL();		//Shift Left						-	8xyE
					break;
				
				default: badOp();
			}
			break;
		case 0x9:
			SNER();					//Skip Not Equal Register		-	9xy0
			break;
		case 0xA:
			LDA();						//Load Address					-	Akkk
			break;
		case 0xB:
			JMPO();					//Jump Offset					-	Bkkk
			break;
		case 0xC:
			RND();						//Generate Random Number		-	Cxkk
			break;
		case 0xD:
			DRW();						//Draw Sprite					-	Dxyn
			break;
		case 0xE:
			if(arg2b == 0x9E){
				SKP();					//Skip If Key Pressed			-	Ex9E
			}
			else{
				SKNP();				//Skip If Key Not Pressed		-	ExA1
			}
			break;
		case 0xF:
			switch(arg2b){
				case 0x07:
					LDD();				//Get Display Timer				-	Fx07
					break;
				case 0x0A:
					LDK();				//Get Pressed Key				-	Fx0A
					break;
				case 0x15:
					SETD();			//Set Display Timer				-	Fx15
					break;
				case 0x18:
					SETS();			//Set Sound Timer				-	Fx18
					break;
				case 0x1E:
					ADDA();			//Add Address					-	Fx1E
					break;
				case 0x29:
					LDF();				//Load font sprite				=	Fx29
					break;
				case 0x33:
					BCD();				//Load Binary-Coded Decimal	-	Fx33
					break;
				case 0x55:
					WRTR();			//Write Registers To Memory	-	Fx55
					break;
				case 0x65:
					GETR();			//Get Registers From Memory	-	Fx65
					break;
				default:
					badOp();
					break;
			}
			break;
		default:
			badOp();
			break;
	}
}

Uint32 timerDecrease(Uint32 interval, void* param){
	if(((Timer*) param)->counter > 0){
		((Timer*) param)->counter--;
	}
	return interval;
}

void printRegisters(){
	printf("Opcode - 0x%04x\nIR - 0x%04x\nPC - 0x%04x\nSP - 0x%04x\nDT - 0x%02x\nST - 0x%02x\n", cpu->op.W, cpu->ir.W, cpu->pc.W, cpu->sp.W, cpu->dt.counter, cpu->st.counter);
	for(int i = 0; i <= 0xF; i++){
		printf("V%d - 0x%02x\n", i, cpu->reg[i]);
	}
}