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

//**********************Enemy_ControlEnemy***********************
// This function do the following tasks:
// - Draws the enemy
// - Moves it randomly  
// - Generates its shoots randomly
// inputs: enemy: Pointer to an element of the enemy array
//         intCounter: Indicates how many cycles of the game engine have occurred
//				 maxY: indicates the maximal Y coordinate the enemy can have
// outputs: none
void Enemy_ControlEnemy(Enemy *this, unsigned long intCounter, unsigned char maxY);


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
