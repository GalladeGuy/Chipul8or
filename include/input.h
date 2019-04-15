#ifndef INPUT_H
#define INPUT_H

#include "memory.h"

#define defaultKeys (int[16]){30, 31, 32, 33, 20, 26, 8, 21, 4, 22, 7, 9, 29, 27, 6, 25}

int keys[16];

void mapKeys(int* array);
void updateKeys();
int isKeyDown(BYTE key);

#endif