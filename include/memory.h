#ifndef MEMORY_H
#define MEMORY_H


typedef unsigned char BYTE;
typedef union{unsigned short int W; struct{BYTE l, h;} B;} WORD;

BYTE* mem;
int romSize;

void initMemory();
void termMemory();
int loadROM(char* path);
void dumpMemory();


#endif