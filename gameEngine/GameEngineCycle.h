#ifndef GAMEENGINEMAIN_H
#define GAMEENGINEMAIN_H

#include <stdbool.h>

// ************************************************************************************************
// ******************************** DEFINITION OF GLOBAL QUANTITIES *******************************
// ************************************************************************************************

#define MAXHP 4
#define MAXGROUND 41  //The ground can be drawn maximal in this Y position


// ************************************************************************************************
// ******************************** DECLARATION OF PUBLIC FUNCTIONS *******************************
// ************************************************************************************************

//**********************GameEngine_Init***********************
// Calls the initialization of the SysTick timer and give
// initial values to some of the game engine variables
// It needs to be called by the main function of the program
// inputs: none
// outputs: none
void GameEngine_Init(void);

//**********************GameEngine_ShowHUD***********************
// Shows the HP and the score of the player at the bottom of the screen
// inputs: none
// outputs: none
void GameEngine_ShowHUD(void);

#endif

