#ifndef CPU_H
#define CPU_H


#include <string.h>
#include "main.h"
#include "gfx.h"
#include "memory.h"
#include "input.h"


#define opArgs (cpu->op.W & 0xFFF)		//Last three nibbles
#define arg1 cpu->arg.B.h					//Second nibble
#define arg2n (cpu->op.B.l >> 4)		//Third nibble
#define arg2n2 (cpu->op.B.l & 0xF)		//Last nibble
#define arg2b cpu->op.B.l					//Last two nibbles


typedef struct{
	SDL_TimerID timer;
	BYTE counter;
}Timer;

typedef struct{
    BYTE reg[0x10];	//General Purpose Registers
    WORD ir;			//Index Register
    WORD pc;			//Program Counter register
    WORD sp;			//Stack Pointer register
    Timer dt;			//Delay Timer
    Timer st;			//Sound Timer
	WORD arg;			//Current instruction arguments
	WORD op;			//Current Opcode
}CPU;

CPU* cpu;

void initCPU();
void termCPU();
void badOp();
void loadOp();
void execOp();
Uint32 timerDecrease(Uint32 interval, void* param);
void resetRegisters();
void printRegisters();

inline void CLS(){		//Clear Screen					-	00E0
	for(int i = 0; i < 2048; i++){
		gfx->fbr[i] = 0;
	}
	displayGraphics();
	cpu->pc.W += 2;
}

inline void RET(){		//Return							-	00EE
	cpu->pc = ((WORD*) mem)[cpu->sp.W];
	cpu->sp.W -= 2;
}

inline void JMP(){		//Jump 							-	1nnn
	cpu->pc.W = cpu->arg.W;
}

inline void CALL(){		//Call								-	2nnn
	cpu->sp.W += 2;
	((WORD*) mem)[cpu->sp.W].W = cpu->pc.W + 2;
	cpu->pc.W = cpu->arg.W;
}

inline void SEI(){		//Skip If Equal Immediate		-	3xkk
	cpu->pc.W += 2 + 2 * (cpu->reg[arg1] == arg2b);
}

inline void SNEI(){		//Skip Not Equal Immediate	-	4xkk
	cpu->pc.W += 2 + 2 * (cpu->reg[arg1] != arg2b);
}

inline void SER(){		//Skip If Equal Register		-	5xy0
	cpu->pc.W += 2 + 2 * (cpu->reg[arg1] == cpu->reg[arg2n]);
}

inline void LDI(){		//Load Immediate				-	6xkk
	cpu->reg[arg1] = arg2b;
	cpu->pc.W += 2;
}

inline void ADI(){		//Add Immediate					-	7xkk
	cpu->reg[arg1] = cpu->reg[arg1] + arg2b;
	cpu->pc.W += 2;
}

inline void LDR(){		//Load Register					-	8xy0
	cpu->reg[arg1] = cpu->reg[arg2n];
	cpu->pc.W += 2;
}

inline void OR(){		//Bitwise OR						-	8xy1
	cpu->reg[arg1] = cpu->reg[arg1] | cpu->reg[arg2n];
	cpu->pc.W += 2;
}

inline void AND(){		//Bitwise AND					-	8xy2
	cpu->reg[arg1] = cpu->reg[arg1] & cpu->reg[arg2n];
	cpu->pc.W += 2;
}

inline void XOR(){		//Bitwise XOR					-	8xy3
	cpu->reg[arg1] = cpu->reg[arg1] ^ cpu->reg[arg2n];
	cpu->pc.W += 2;
}

inline void ADDR(){		//Add Registers					-	8xy4
	cpu->reg[0xF] = __builtin_add_overflow(cpu->reg[arg1], cpu->reg[arg2n], &cpu->reg[arg1]);
	cpu->pc.W += 2;
}

inline void SUB(){		//Subtract (Vx - Vy)			-	8xy5
	cpu->reg[0xF] = __builtin_sub_overflow(cpu->reg[arg1], cpu->reg[arg2n], &cpu->reg[arg1]);
	cpu->pc.W += 2;
}

inline void SHR(){		//Shift Right					-	8xy6
	cpu->reg[0xF] = cpu->reg[arg2n] & 1;
	cpu->reg[arg1] >>= 1;
	cpu->pc.W += 2;
}

inline void SUBN(){		//Subtract (Vy - Vx)			-	8xy7
	cpu->reg[0xF] = __builtin_sub_overflow(cpu->reg[arg2n], cpu->reg[arg1], &cpu->reg[arg1]) ^ 1;
	cpu->pc.W += 2;
}

inline void SHL(){		//Shift Left						-	8xyE
	cpu->reg[0xF] = (cpu->reg[arg1] & 0x80) >> 7;
	cpu->reg[arg1] <<= 1;
	cpu->pc.W += 2;
}

inline void SNER(){		//Skip Not Equal Register		-	9xy0
	cpu->pc.W += 2 + 2 * (cpu->reg[arg1] != cpu->reg[arg2n]);
}

inline void LDA(){		//Load Address					-	Akkk
	cpu->ir.W = cpu->arg.W;
	cpu->pc.W += 2;
}

inline void JMPO(){		//Jump Offset					-	Bkkk
	cpu->pc.W = cpu->arg.W + cpu->reg[0];
}

inline void RND(){		//Generate Random Number		-	Cxkk
	cpu->reg[arg1] = (BYTE)rand() & arg2b;
	cpu->pc.W += 2;
}

inline void DRW(){		//Draw Sprite					-	Dxyn
	cpu->reg[0xF] = 0;
	for(int y = 0; y < arg2n2; y++){
		for(int x = 0; x < 8; x++){
			int pos = ((cpu->reg[arg1] + x) & 63) + (((y + cpu->reg[arg2n]) & 31) << 6);
			if(mem[cpu->ir.W + y] & (0x80 >> x)){
				gfx->fbr[pos] ^= 0xFF;
				cpu->reg[0xF] |= (!gfx->fbr[pos]);
			}
		}
	}
	SDL_UpdateTexture(gfx->t, NULL, gfx->fbr, 64);
	displayGraphics();
	cpu->pc.W += 2;
}

inline void SKP(){		//Skip If Key Pressed			-	Ex9E
	updateKeys();
	cpu->pc.W += 2 + 2 * isKeyDown(arg1);
}

inline void SKNP(){		//Skip If Key Not Pressed		-	ExA1
	updateKeys();
	cpu->pc.W += 2 + 2 * !isKeyDown(arg1);
}

inline void LDD(){		//Get Display Timer				-	Fx07
	cpu->reg[arg1] = cpu->dt.counter;
	cpu->pc.W += 2;
}

inline void LDK(){		//Get Pressed Key				-	Fx0A
	while(1){
		updateKeys();
		for(BYTE i = 0; i < 16; i++){
			if(isKeyDown(i)){
				cpu->reg[arg1] = i;
				goto end;
			}
		}
	}
	end:
	cpu->pc.W += 2;
}

inline void SETD(){		//Set Display Timer				-	Fx15
	cpu->dt.counter = cpu->reg[arg1];
	cpu->pc.W += 2;
}

inline void SETS(){		//Set Sound Timer				-	Fx18
	cpu->st.counter = cpu->reg[arg1];
	cpu->pc.W += 2;
}

inline void ADDA(){		//Add Address					-	Fx1E
	cpu->ir.W += cpu->reg[arg1];
	cpu->pc.W += 2;
}

inline void LDF(){		//Load font sprite				=	Fx29
	cpu->ir.W = 5 * cpu->reg[arg1];
	cpu->pc.W += 2;
}

inline void BCD(){		//Load Binary-Coded Decimal	-	Fx33
	mem[cpu->ir.W] = arg1 / 100;
	mem[cpu->ir.W + 1] = (arg1 / 10) % 10;
	mem[cpu->ir.W + 2] = arg1 % 10;
	cpu->pc.W += 2;
}

inline void WRTR(){		//Write Registers To Memory	-	Fx55
	memcpy(&mem[cpu->ir.W], cpu->reg, arg1);
	cpu->pc.W += 2;
}

inline void GETR(){		//Get Registers From Memory	-	Fx65
	memcpy(cpu->reg, &mem[cpu->ir.W], arg1);
	cpu->pc.W += 2;
}


#endif