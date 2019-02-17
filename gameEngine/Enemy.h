#ifndef ENEMY_H
#define ENEMY_H

// ************************************************************************************************
// *********************************** DECLARATION OF "CLASS" *************************************
// ************************************************************************************************
typedef struct enemyVariables Enemy;

struct enemyVariables
{
	unsigned char posX;
	unsigned char posY;
	unsigned char dead;
	unsigned char *image[5];
	unsigned char actStatus;	
};

// ************************************************************************************************
// ******************************** DECLARATION OF PUBLIC FUNCTIONS *******************************
// ************************************************************************************************

//**********************_ControlEnemy***********************
// This function do the following tasks:
// - Draws the enemy
// - Moves it randomly  
// - Generates its shoots randomly
// inputs: enemy: Pointer to an element of the enemy array
//         intCounter: interrupt counter
// outputs: none
void Enemy_ControlEnemy(Enemy *this, unsigned long intCounter, unsigned char maxY);


//**********************_StartEnemy***********************
// This function initializes the enemy variables
// inputs: enemy: Pointer to an element of the enemy array
// outputs: none
void Enemy_StartEnemy(Enemy *this);

#endif
