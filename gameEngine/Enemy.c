
// ************************************************************************************************
// ******************************* INTERACTION WITH ANOTHER MODULES *******************************
// ************************************************************************************************
#include "Enemy.h"
#include "../Display/Nokia5110.h"
#include "../Sounds/Sound.h"
#include "../Main/Random.h"

// ************************************************************************************************
// ******************************** DEFINITION OF GLOBAL QUANTITIES *******************************
// ************************************************************************************************
#define ENEMYW 12
#define ENEMYH 8


// ************************************************************************************************
// ****************************** DECLARATION OF FINITE STATE MACHINES ****************************
// ************************************************************************************************

//********** Finite state machine for enemy animation **************
typedef const struct enemyFSM_Variables
{
	unsigned char imgNumber;
	unsigned short delay;
	unsigned char next[2];
}enemyFSM_Type;

#define enemyFSM_Alive1 0
#define enemyFSM_Alive2 1
#define enemyFSM_Alive3 2
#define enemyFSM_Dying1 3
#define enemyFSM_Dying2 4
#define enemyFSM_NoShow 5

enemyFSM_Type enemyFSM[6] =
{
	/* STATE: Alive1 */ {0, 50, {enemyFSM_Alive2, enemyFSM_Dying1}},
	/* STATE: Alive2 */ {1, 10, {enemyFSM_Alive3, enemyFSM_Dying1}},
	/* STATE: Alive3 */ {2, 10, {enemyFSM_Alive1, enemyFSM_Dying1}},
	/* STATE: Dying1 */ {3, 20, {enemyFSM_Dying2, enemyFSM_Dying2}},
	/* STATE: Dying2 */ {4, 20, {enemyFSM_NoShow, enemyFSM_NoShow}},
	/* STATE: NoShow */ {5, 0, {enemyFSM_NoShow, enemyFSM_NoShow}}
};


// ************************************************************************************************
// *********************************** DEFINITION OF FUNCTIONS ************************************
// ************************************************************************************************

//**********************Enemy_InitEnemy***********************
// This function initializes the enemy variables
// inputs: enemy: Pointer to an element of the enemy array
// 				 img0:  Enemy centered
//				 img1:	Enemy moving left
//				 img2: 	Enemy moving right
//				 img3: 	Image 1 of destruction of enemy
//				 img4:	Image 2 of destruction of enemy
// outputs: none
void Enemy_InitEnemy(Enemy *this, const unsigned char *img0, const unsigned char *img1, const unsigned char *img2, const unsigned char *img3, const unsigned char *img4)
{
		this->posX = SCREENW - ENEMYW - 2; 							// We initialize all enemies to be at the right border of the screen
		this->posY = 10;
		this->dead = 0;
		this->image[0] = (unsigned char*)img0;  // We point the animation arrays to the individual images on "ImageArrays.h"
		this->image[1] = (unsigned char*)img1;  
		this->image[2] = (unsigned char*)img2;
		this->image[3] = (unsigned char*)img3;
		this->image[4] = (unsigned char*)img4;
		this->actStatus = enemyFSM_NoShow;								// We don't show the enemies at the beginning
}

//**********************Enemy_ControlEnemy***********************
// This function do the following tasks:
// - Draws the enemy
// - Moves it randomly  
// - Generates its shoots randomly
// inputs: enemy: Pointer to an element of the enemy array
//         intCounter: Indicates how many cycles of the game engine have occurred
//				 maxY: indicates the maximal Y coordinate the enemy can have
// outputs: none
void Enemy_ControlEnemy(Enemy *this, unsigned long intCounter, unsigned char maxY)
{
	// %%%%%%%%%%%%%% CONTROL OF DEATH %%%%%%%%%%%%%%%%
	unsigned char i = this->posY;
	unsigned char oldStatus = this->actStatus;		//Variable used to determine the next FSM status
	unsigned char nextPosX = this->posX;												//Next position in X calculated randomly
	unsigned char nextPosY = this->posY;												//Next position in Y calculated randomly
	bool moveEnemy = true;
	
	while ((i > this->posY - ENEMYH) && (this->dead == 0))	//i corresponds to the coordinates in Y axis from the bottom of
	{																												//the enemy image to its top
		if (Nokia5110_AskPixel(this->posX - 2, i))						//If a pixel is turned on 2 pixels before the enemy image
		{																											//the enemy is killed
			this->dead = 1;
		}
		i--;
	}
	// %%%%%%%%%%%%%% FSM's NEXT STATUS %%%%%%%%%%%%%%%%%%
	if (intCounter%enemyFSM[oldStatus].delay == 0)							//It changes the status of the FSM every number of interrupts
	{																														//Given by the delay of the current status
		this->actStatus = enemyFSM[oldStatus].next[this->dead];		//The next status is determined by the output of the FSM corresponding to the input (this->dead)
	}
	// %%%%%%%%%%%%%% enemy's next position %%%%%%%%%%%%%%
	if ((this->actStatus < 3) && (intCounter%5 == 0))										//Every 5 interrupts and only if the current status is smaller than 3 (which means alive)
	{
		if ((this->posX > ENEMYW) && (this->posX < SCREENW - ENEMYW - 2))	//If the current position in X is larger than the width of the enemy image and shorter than  
		{																																 	//the width of the screen minus the width of the enemy -2  
																											
			nextPosX = this->posX - 1 + Random()%3;													//nextPosX is equal to this->posX - 1, this->PosX or this->PosX + 1
		}
		if ((this->posY > ENEMYH) && (this->posY < maxY))
		{
			nextPosY = this->posY - 1 + Random()%3;
		}
		i = nextPosY;
		while ((i > nextPosY - ENEMYH) && moveEnemy)
		{
			moveEnemy &= ~Nokia5110_AskPixel(nextPosX - 2, i) & ~Nokia5110_AskPixel(nextPosX + 2 + ENEMYW, i);
			i--;
		}
		if (moveEnemy)
		{
			this->posX = nextPosX;
			this->posY = nextPosY;
		}
	}
	
	// %%%%%%%%%%%%%% DRAW ENEMY %%%%%%%%%%%%%%%%
	if (this->actStatus != enemyFSM_NoShow)
	{
		Nokia5110_PrintBMP(this->posX, this->posY, this->image[this->actStatus], 0);
	}
	// %%%%%%%%%%%%%% CREATE NEW ENEMY %%%%%%%%%%%
	else
	{
		unsigned char createNewEnemy = Random()%10;
		if (createNewEnemy == 1)
		{
			this->actStatus = enemyFSM_Alive1;
			this->posX = 2*ENEMYW + Random()%(SCREENW - 3*ENEMYW);
			this->posY = ENEMYH + Random()%(maxY - ENEMYH);
			if (Nokia5110_AskPixel(this->posX, this->posY))
			{
				this->posY -= 20;
			}
			this->dead = 0;
			Sound_Highpitch();
		}
	}	
}

