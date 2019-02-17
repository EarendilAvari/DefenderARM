#ifndef GAMEENGINEMAIN_H
#define GAMEENGINEMAIN_H

#include <stdbool.h>


//**********************GameEngine_Init***********************
// Calls the initialization of the SysTick timer and give
// initial values to some of the game engine variables
// It needs to be called by the main function of the program
// inputs: none
// outputs: none
void GameEngine_Init(void);

#endif
