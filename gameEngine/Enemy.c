
// ************************************************************************************************
// ******************************* INTERACTION WITH ANOTHER MODULES *******************************
// ************************************************************************************************
#include "Enemy.h"

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

//**********************Enemy_ControlDeath***********************
// This function controls the death of an enemy pointed by "this".
// It determines when the enemy should die based on the pixels that 
// are already turned on
// inputs: enemy: Pointer to an element of the enemy array
// outputs: 1: Enemy was killed, 0: Enemy was not killed
bool Enemy_ControlDeath(Enemy *this)
{
	unsigned char i = this->posY;
	while ((i > this->posY - ENEMYH) && (this->dead == 0))	//i corresponds to the coordinates in Y axis from the bottom of
	{																												//the enemy image to its top
		if (Nokia5110_AskPixel(this->posX - 2, i) && (this->shoots.PosX < this->posX - 4))		//If a pixel is turned on 2 pixels before the enemy image
		{																											//the enemy is killed
			this->dead = 1;
			Sound_Killed();
			return 1;
		}
		i--;
	}
	return 0;
}


//**********************Enemy_NextState***********************
// This function determines the next state of the final state machine.
// for the enemy pointed by this. So it determines which picture should
// be shown next
// inputs: enemy: Pointer to an element of the enemy array
//         intCounter: Indicates how many cycles of the game engine have occurred
// outputs: none
void Enemy_NextState(Enemy *this, unsigned long intCounter)
{
	unsigned char oldStatus = this->actStatus;		//Variable used to determine the next FSM status
	if (intCounter%enemyFSM[oldStatus].delay == 0)							//It changes the status of the FSM every number of interrupts
	{																														//Given by the delay of the current status
		this->actStatus = enemyFSM[oldStatus].next[this->dead];		//The next status is determined by the output of the FSM corresponding to the input (this->dead)
	}
}


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
void Enemy_NextPos(Enemy *this, unsigned long intCounter, unsigned char maxY)
{
	unsigned char i;
	unsigned char nextPosX = this->posX;												//Next position in X calculated randomly, initialized with the current position on X
	unsigned char nextPosY = this->posY;												//Next position in Y calculated randomly, initialized with the current position on Y
	bool moveEnemy = true;
	if ((this->actStatus < 3) && (intCounter%5 == 0))										//Every 5 interrupts and only if the current status is smaller than 3 (which means alive)
	{
		// The next position of the enemy is randomly determined, 
		// If the enemy is in a border it can only move away from it or 
		// stay in there.
		// The enemy can move in the square delimited by ENEMYW at the left
		// SCREENW at the right, 0 at the top, and maxY at the bottom
		if (this->posX < ENEMYW)
		{
			nextPosX = this->posX + Random()%2;
		}
		else if (this->posX > SCREENW - ENEMYW - 2)
		{
			nextPosX = this->posX - 1 + Random()%2;
		}
		else 
		{
			nextPosX = this->posX - 1 + Random()%3;
		}
		
		if (this->posY < ENEMYH)
		{
			nextPosY = this->posY + Random()%2;
		}
		else if (this->posY > maxY)
		{
			nextPosY = this->posY - 1 + Random()%2;
		}
		else 
		{
			nextPosY = this->posY - 1 + Random()%3;
		}
		
		i = nextPosY;
		while ((i > nextPosY - ENEMYH) && moveEnemy)											// it is checked if there is something 2 pixels to the right and two pixels to the left of the 
		{																																	// image, for the entire height of the image
			moveEnemy &= ~Nokia5110_AskLastPixel(nextPosX - 2, i) & ~Nokia5110_AskLastPixel(nextPosX + 2 + ENEMYW, i);
			i--;
		}
		if (moveEnemy)																										// If there is nothing, the enemy is moved to the position calculated before
		{
			this->posX = nextPosX;
			this->posY = nextPosY;
		}
	}
}


//**********************Enemy_Draw***********************
// This function whether draws the enemy into the display buffer if it should be shown
// (based on the state of the finite state machine) or it shows it again if is not shown 
// now because it was killed or because the game was just started.
// To do that, a random number between 0 and 49 is asked, and if it results to be 1
// the new enemy is created.
// inputs: enemy: Pointer to an element of the enemy array
//				 maxY: indicates the maximal Y coordinate the enemy can have
//				 diff: difficulty of the game
// outputs: none
void Enemy_Draw(Enemy *this, unsigned char maxY, unsigned short diff)
{
	unsigned char i, j;
	// %%%%%%%%%%%%%% DRAW ENEMY %%%%%%%%%%%%%%%%
	if (this->actStatus != enemyFSM_NoShow)							//If the current status of the enemy is different to "not show", we print it to the display
	{
		Nokia5110_PrintBMP(this->posX, this->posY, this->image[this->actStatus], 0);
	}
	// %%%%%%%%%%%%%% CREATE NEW ENEMY %%%%%%%%%%%
	else																								//If the enemy is not being shown (meaning it was killed and it did not come back yet
	{
		unsigned char createNewEnemy = Random()%(70 - diff);				// There is a probability of 1/50 to create an enemy in this cycle
		bool safeToDraw = true;
		if (createNewEnemy == 1)
		{
			this->posX = 2*ENEMYW + Random()%(SCREENW - 3*ENEMYW);	//The enemy is created in a random position on X between 2*ENEMYW and SCREENW-ENEMYW
			this->posY = ENEMYH + Random()%(maxY - ENEMYH);					//The enemy is created in a random position on Y between ENEMYH and maxY
			// The entire area that will be used by the new enemy is ckecked
			// If there is already an enemy in that place, the enemy is not created at this cycle
			// In order to do that the variable safeToDraw is used
			for (i = this->posX - 2; i < (this->posX + ENEMYW); i++)
			{
				for (j = this->posY; j > (this->posY-ENEMYH); j--)
				{
					safeToDraw &= !Nokia5110_AskLastPixel(i, j);
				}
			}
			if (safeToDraw)
			{
				this->actStatus = enemyFSM_Alive1;
				this->dead = 0;
				Sound_Highpitch();
			}
		}	
	}	
}

//**********************Enemy_Shoots***********************
// This function generates the shoots of the enemy pointed by this.
// The shoots are shown until they reach the right border of the display.
// They are not generated if another enemy is on the way of the shoot in order to avoid
// enemies killing another enemies.
// inputs: enemy: Pointer to an element of the enemy array
//				 diff: difficulty of the game
// outputs: none
void Enemy_Shoots(Enemy *this, unsigned short diff)
{
	bool noOtherEnemy = true;
	bool collisionDetected = false;
	unsigned char i = this->posX - 1;																							 	// It evaluates if the way to the player ship is free of other enemies, if is not, it does not 
	while (i > 10 && noOtherEnemy)																		// not shoot, so it doesn't kill another enemies
	{
		noOtherEnemy &= ~Nokia5110_AskPixel(i, this->posY - ENEMYH/2);
		i--;
	}
	
	if ((Random()%(70 - 2*diff) == 0) && !this->shoots.show && this->actStatus != enemyFSM_NoShow && noOtherEnemy)	// If there is not a shoot being shown, and the current status 
	{																																							// of the enemy is not not shown (in other words it is not dead) and we get a 1 for a 
		this->shoots.show = true;																										// random number between 0 and 49. It shoots.
		Sound_Shoot();
		this->shoots.PosX = this->posX - 1;
		this->shoots.PosY = this->posY - ENEMYH/2;
	}

	if (this->shoots.show)																							// We show the shoot until it goes out of the screen
	{
		Nokia5110_SetPixel(this->shoots.PosX, this->shoots.PosY);
		Nokia5110_SetPixel(this->shoots.PosX - 1, this->shoots.PosY);
		Nokia5110_SetPixel(this->shoots.PosX - 2, this->shoots.PosY);
		this->shoots.PosX--;		
		collisionDetected = Nokia5110_AskPixel(this->shoots.PosX-2, this->shoots.PosY);
		if (this->shoots.PosX <= 2 || collisionDetected)
		{
			this->shoots.show = false;
		}
	}
}

//**********************Enemy_Reset***********************
// This functions clear the enemies in order to avoid killings by them when 
// the shoot is still being shown on the display and it already killed the player
// ship.
// inputs: enemy: Pointer to an element of the enemy array
// outputs: none
void Enemy_Reset(Enemy *this)
{
	this->actStatus = enemyFSM_NoShow;
	this->shoots.show = false;
}
