#ifndef ENEMY_H
#define ENEMY_H

#include "../Display/Nokia5110.h"
#include "../Sounds/Sound.h"
#include "../Main/Random.h"
#include "Pixel.h"

// ************************************************************************************************
// *********************************** DECLARATION OF "CLASS" *************************************
// ************************************************************************************************
typedef struct enemyVariables Enemy;

struct enemyVariables
{
	unsigned char posX;
	unsigned char posY;
	PixelType shoots;				//Array of shoots of the enemy
	unsigned char dead;
	unsigned char *image[5];
	unsigned char actStatus;	
};

// ************************************************************************************************
// ******************************** DECLARATION OF PUBLIC FUNCTIONS *******************************
// ************************************************************************************************

//**********************Enemy_ControlDeath***********************
// This function controls the death of an enemy pointed by "this".
// It determines when the enemy should die based on the pixels that 
// are already turned on
// inputs: enemy: Pointer to an element of the enemy array
// outputs: 1: Enemy was killed, 0: Enemy was not killed
bool Enemy_ControlDeath(Enemy *this);


//**********************Enemy_NextState***********************
// This function determines the next state of the final state machine.
// for the enemy pointed by this. So it determines which picture should
// be shown next
// inputs: enemy: Pointer to an element of the enemy array
//         intCounter: Indicates how many cycles of the game engine have occurred
// outputs: none
void Enemy_NextState(Enemy *this, unsigned long intCounter);


//**********************Enemy_NextPos***********************
// This function determines the next position of the enemy pointed by this.
// The position is two dimensional and can variate between the current position
// minus one pixel and the current position plus one pixel. 
// So the movement up, down, left and right is made possible.
// Whether the position changes to one direction or to the other is generated randomly.
// inputs: enemy: Pointer to an element of the enemy array
//         intCounter: Indicates how many cycles of the game engine have occurred
//				 maxY: indicates the maximal Y coordinate the enemy can have
// outputs: none
void Enemy_NextPos(Enemy *this, unsigned long intCounter, unsigned char maxY);


//**********************Enemy_Draw***********************
// This function whether draws the enemy into the display buffer if it should be shown
// (based on the state of the finite state machine) or it shows it again if is not shown 
// now because it was killed or because the game was just started.
// To do that, a random number between 0 and 49 is asked, and if it results to be 1
// the new enemy is created.
// inputs: enemy: Pointer to an element of the enemy array
//				 maxY: indicates the maximal Y coordinate the enemy can have
// outputs: none
void Enemy_Draw(Enemy *this, unsigned char maxY);


//**********************Enemy_Shoots***********************
// This function generates the shoots of the enemy pointed by this.
// The shoots are shown until they reach the right border of the display.
// They are not generated if another enemy is on the way of the shoot in order to avoid
// enemies killing another enemies.
// inputs: enemy: Pointer to an element of the enemy array
// outputs: none
void Enemy_Shoots(Enemy *this);

//**********************Enemy_Reset***********************
// This functions clear the enemies in order to avoid killings by them when 
// the shoot is still being shown on the display and it already killed the player
// ship.
// inputs: enemy: Pointer to an element of the enemy array
// outputs: none
void Enemy_Reset(Enemy *this);

//**********************Enemy_InitEnemy***********************
// This function initializes the enemy variables
// inputs: enemy: Pointer to an element of the enemy array
// 				 img0:  Enemy centered
//				 img1:	Enemy moving left
//				 img2: 	Enemy moving right
//				 img3: 	Image 1 of destruction of enemy
//				 img4:	Image 2 of destruction of enemy
// outputs: none
void Enemy_InitEnemy(Enemy *this, const unsigned char *img0, const unsigned char *img1, const unsigned char *img2, const unsigned char *img3, const unsigned char *img4);

#endif
