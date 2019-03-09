#ifndef GAMEENGINEMAIN_H
#define GAMEENGINEMAIN_H

#include <stdbool.h>
#include "GameEngineCycle.h"
#include "Pixel.h"
#include "Enemy.h"
#include "Terrain.h"
#include "PlayerShip.h"

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
// inputs: timerSpecialAttack: time needed to unlock special attack
//				 intCounter: Indicates how many cycles of the game engine have occurred
// outputs: none
void GameEngine_ShowHUD(unsigned long intCounter, unsigned short timerSpecialAttack);

//**********************GameEngine_IncreaseDifficulty***********************
// Shows the HP and the score of the player at the bottom of the screen
// inputs: player: Pointer to the playership object
//				 diff: Pointer to difficulty variable
// outputs: none
void GameEngine_IncreaseDifficulty(PlayerShip* player, unsigned short* diff);

//**********************GameEngine_IncreaseGroundHeight***********************
// Increases the maximum height of the ground depending on the difficulty
// inputs: player: Pointer to the playership object
//				 diff: Pointer to difficulty variable
// outputs: none
void GameEngine_IncreaseGroundHeight(unsigned short* GroundHeight, unsigned short diff);

#endif

