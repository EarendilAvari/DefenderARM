// ************************************************************************************************
// ******************************* INTERACTION WITH ANOTHER MODULES *******************************
// ************************************************************************************************
#include "PlayerShip.h"

// ************************************************************************************************
// ******************************** DEFINITION OF GLOBAL QUANTITIES *******************************
// ************************************************************************************************
#define MAXHP 4
#define SHIPW	10
#define SHIPH	7 
#define SPECIAL_SHOOT_MID_W 10
#define SPECIAL_SHOOT_MID_H 5
#define SPECIAL_SHOOT_SIDE_W 8
#define SPECIAL_SHOOT_SIDE_H 9

// ************************************************************************************************
// ****************************** DECLARATION OF FINITE STATE MACHINES ****************************
// ************************************************************************************************

//********** Finite state machine for ship animation **************
typedef const struct shipFSM_Variables
{
	unsigned char imgNumber;
	unsigned short delay;
	unsigned char next[8];
}shipFSM_Type;


#define shipFSM_Center 0
#define shipFSM_Up 1
#define shipFSM_Down 2
#define shipFSM_Dying1 3
#define shipFSM_Dying2 4
#define shipFSM_Dying3 5
#define shipFSM_Dead 6

shipFSM_Type PlayerShipFSM[7] =
{
	/* STATE: Center */ {0, 1, {shipFSM_Center, shipFSM_Dying1, shipFSM_Up, shipFSM_Dying1, 
															 shipFSM_Down, shipFSM_Dying1, shipFSM_Center, shipFSM_Dying1}},
	/* STATE: Up		 */ {1, 10, {shipFSM_Center, shipFSM_Dying1, shipFSM_Up, shipFSM_Dying1, 
															 shipFSM_Down, shipFSM_Dying1, shipFSM_Up, shipFSM_Dying1}},
	/* STATE: Down	 */ {2, 10, {shipFSM_Center, shipFSM_Dying1, shipFSM_Up, shipFSM_Dying1, 
															 shipFSM_Down, shipFSM_Dying1, shipFSM_Down, shipFSM_Dying1}},
	/* STATE: Dying1 */ {3, 10, {shipFSM_Dying2, shipFSM_Dying2, shipFSM_Dying2, shipFSM_Dying2, 
															 shipFSM_Dying2, shipFSM_Dying2, shipFSM_Dying2, shipFSM_Dying2}},
	/* STATE: Dying2 */ {4, 10, {shipFSM_Dying3, shipFSM_Dying3, shipFSM_Dying3, shipFSM_Dying3, 
															 shipFSM_Dying3, shipFSM_Dying3, shipFSM_Dying3, shipFSM_Dying3}},
	/* STATE: Dying3 */ {5, 10, 	{shipFSM_Dead, shipFSM_Dead, shipFSM_Dead, shipFSM_Dead, 
															 shipFSM_Dead, shipFSM_Dead, shipFSM_Dead, shipFSM_Dead}},
	/* STATE: Dead	 */ {6, 0, 	{shipFSM_Center, shipFSM_Center, shipFSM_Center, shipFSM_Center, 
															 shipFSM_Center, shipFSM_Center, shipFSM_Center, shipFSM_Center}}
};

// ************************************************************************************************
// *********************************** DEFINITION OF FUNCTIONS ************************************
// ************************************************************************************************

//**********************PlayerShip_InitShip***********************
// This function do the following tasks:
// - Draws the ship and moves it on dependance of the slide pot
// - Generate the shoots of the ship on dependance of the shoot switch
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 img0: image of the ship centered
//				 img1: image of the ship going up
//				 img2: image of the ship going down
// 				 img3: image 1 of destruction animation
//				 img4: image 2 of destruction animation
// 				 img5: image 3 of destruction animation
// outputs: none
void PlayerShip_InitShip(PlayerShip* this, const unsigned char *img0, const unsigned char *img1, const unsigned char *img2,
												 const unsigned char *img3, const unsigned char *img4, const unsigned char *img5)
{
		unsigned char i;
	  for (i=0; i<5; i++)
		{
			this->shoots[i].PosX = SHIPW;	// It makes the position of the shoots to be at Y=0 and
			this->shoots[i].PosY = 0;			// X = the width of the ship
		}
		this->posX = 0;
		this->posY = 30;
		this->shCounter = 0;			// We set the quantity of shown shoots to be 0
		this->healthPoints = 3;	// Initial HP of the ship
		this->score = 0;					// Initial score of the ship
		this->dead = 0;
		this->image[0] = (unsigned char*)img0;  // We point the animation arrays to the individual images on "ImageArrays.h"
		this->image[1] = (unsigned char*)img1;  
		this->image[2] = (unsigned char*)img2;
		this->image[3] = (unsigned char*)img3;
		this->image[4] = (unsigned char*)img4;
		this->image[5] = (unsigned char*)img5;
		this->curStatus = shipFSM_Center;								// We don't show the enemies at the beginning
}

//**********************PlayerShip_InitSpecialShoot***********************
// This function initializes the special shoots
// This function is separated from the rest of the player ship initialization because a lot of images are
// used to create this special shoot
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 imgMid1:  image of the special shoot in the middle 1
//				 imgMid2:  image of the special shoot in the middle 2
//				 imgUp1: 	 image of the special shoot in the top 1
// 				 imgUp2: 	 image of the special shoot in the top 2
//				 imgDown1: image of the special shoot in the bottom 1
// 				 imgDown2: image of the special shoot in the bottom 2
// outputs: none
void PlayerShip_InitSpecialShoot(PlayerShip* this, const unsigned char* imgMid1, const unsigned char* imgMid2,
																									 const unsigned char* imgUp1, const unsigned char* imgUp2,
																									 const unsigned char* imgDown1, const unsigned char* imgDown2)
{
		this->specialShootMiddle.show = false;
		this->specialShootUp.show = false;
		this->specialShootDown.show = false;
		this->specialShootMiddle.image[0] = (unsigned char*)imgMid1;
		this->specialShootMiddle.image[1] = (unsigned char*)imgMid2;
		this->specialShootUp.image[0] = (unsigned char*)imgUp1;
		this->specialShootUp.image[1] = (unsigned char*)imgUp2;
		this->specialShootDown.image[0] = (unsigned char*)imgDown1;
		this->specialShootDown.image[1] = (unsigned char*)imgDown2;
}



//**********************PlayerShip_ControlShip***********************
// This function performs the following tasks:
// - Determines the next position of the ship
// - Determines whether the player ship is killed in this cycle or not
// - Changes the status of the finite state machine used for the ship
// based on the input from the slide pot and if it is dying or not.
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 intCounter: Indicates how many cycles of the game engine have occurred
// outputs: none
void PlayerShip_ControlShip(PlayerShip *this, unsigned char intCounter)
{
	unsigned char i;
	static unsigned char PixelY_last;
	bool MovedUp = false;
	bool MovedDown = false;
	bool Hurt = false;
	unsigned char input = 0;
	unsigned char oldStatus = this->curStatus;
	bool Touched = false;
	this->posY = SlidePot_toPixelY(SHIPH);							// Converts the ADC data into readable distance value and then to a position in the Y axis
	
	i = this->posY;
	while ((i > this->posY - SHIPH) && !Touched) 
	{
		Touched |= Nokia5110_AskLastPixel(SHIPW, i);
		i--;
	}
	if (Touched)					// or if pixel for the superior peak is already set
	{
		Hurt = true;
	}
	
	if (this->posY > PixelY_last)
	{
		MovedDown = true;
	}
	
	if (this->posY < PixelY_last)
	{
		MovedUp = true;
	}
	
	PixelY_last = this->posY;
	
	input = Hurt + (MovedUp<<1) + (MovedDown<<2);
	
	if (intCounter%PlayerShipFSM[oldStatus].delay == 0)
	{
		this->curStatus = PlayerShipFSM[oldStatus].next[input];
	}
	
	if (this->curStatus == shipFSM_Dead)
	{
		this->dead = 2;
		this->healthPoints--;
		Sound_Explosion();
		return;
	}
}

//**********************PlayerShip_Draw***********************
// This function whether draws the player ship on the display buffer
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
void PlayerShip_Draw(PlayerShip *this)
{
	Nokia5110_PrintBMP(0, this->posY, this->image[this->curStatus], 0);  		// Draws the ship in the display using the value from the slide pot
}


//**********************PlayerShip_Shoots***********************
// This function generates the shoots of the player ship pointed by this.
// The shoots are shown until they reach the right border of the display.
// The ship can generate until 5 shoots at the same time
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
void PlayerShip_Shoots(PlayerShip *this)
{
	unsigned char i;
	bool collisionDetected = false;
	if (Switch_shoot)																				// If the switch is pressed we do the following operations on the next shoot on the array
	{
		this->shoots[this->shCounter].show = true;							// We make showingShoot true, this variable is used to show the shoot in the display until it dissapears
		Sound_Shoot();																															// We play the shoot sound by pressing the switch
		this->shoots[this->shCounter].PosY = this->posY - (SHIPH/2);  // We set the position Y of the shoot equals to the center of the ship
		this->shoots[this->shCounter].PosX = SHIPW + 1;								// We set the position X of the shoot equals to the pick of the ship
		this->shCounter++;
		if (this->shCounter >= 5) this->shCounter = 0;							// If ShootCounter is bigger or equal to 5, it is setted again to 0
		Switch_shoot = false;																	// We set back the value of the switch
	}
	for (i=0; i<5; i++)						// For every possible shoot
	{
		if (this->shoots[i].show)		// If the shoot should be shown
		{
			Nokia5110_SetPixel(this->shoots[i].PosX, this->shoots[i].PosY);				// We print three pixels which move until they go out of the screen
			Nokia5110_SetPixel(this->shoots[i].PosX, this->shoots[i].PosY + 1);
			Nokia5110_SetPixel(this->shoots[i].PosX + 1, this->shoots[i].PosY);
			Nokia5110_SetPixel(this->shoots[i].PosX + 1, this->shoots[i].PosY + 1);
			//Nokia5110_SetPixel(this->shoots[i].PosX + 2, this->shoots[i].PosY);
			this->shoots[i].PosX++;									
			collisionDetected = Nokia5110_AskLastPixel(this->shoots[i].PosX, this->shoots[i].PosY + 1) || 
													Nokia5110_AskLastPixel(this->shoots[i].PosX + 1, this->shoots[i].PosY + 1);
			if (this->shoots[i].PosX >= SCREENW || collisionDetected)
			{
				this->shoots[i].show = false;													// if the shoot reaches the end of the screen, we turn off the value indicating that a shoot should be shown
			}
		}
	}
}

//**********************PlayerShip_specialShoots***********************
// This function generates a special shoot by pressing the special button
// The special shoot is a big misile going through the X axis and also two misiles going up and down with a 45 
// degrees angle
// The three misiles use two BMPs to create an animation. In order to create that animation a finite state
// machine without inputs and two states is used. The status switches between 0 and 1 every 6 SysTick interrupts 
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 intCounter: Indicates how many cycles of the game engine have occurred
//				 maxY: indicates the maximal Y coordinate the shoots can have
//				 timerSpecialAttack: time needed to unlock special attack
// outputs: none
void PlayerShip_specialShoot(PlayerShip *this, unsigned long intCounter, unsigned short* timerSpecialAttack, unsigned char maxY)
{
	if (Switch_special)
	{
		// If the switch special is pressed, a new special shoot is generated
		// with position at the center, top and bottom of the player ship
		this->specialShootMiddle.show = true;
		this->specialShootUp.show = true;
		this->specialShootDown.show = true;
		this->specialShootMiddle.posY = this->posY - (SHIPH/2);
		this->specialShootUp.posY = this->posY - SHIPH;
		this->specialShootDown.posY = this->posY;
		this->specialShootMiddle.posX = SHIPW + 1;
		this->specialShootUp.posX = SHIPW + 1;
		this->specialShootDown.posX = SHIPW + 1;
		this->specialShootMiddle.curStatus = 0;
		Switch_special = false;
	}
	
	if (this->specialShootMiddle.show)
	{
		Nokia5110_PrintBMP(this->specialShootMiddle.posX, this->specialShootMiddle.posY, 
							this->specialShootMiddle.image[this->specialShootMiddle.curStatus], 0);
		this->specialShootMiddle.posX++;
		if (intCounter%6 == 0)
		{
			this->specialShootMiddle.curStatus = (this->specialShootMiddle.curStatus + 1)&0x01;
			Sound_Fastinvader1();
		}
		if (this->specialShootMiddle.posX > SCREENW - SPECIAL_SHOOT_MID_W)
		{
			this->specialShootMiddle.show = false;
			*timerSpecialAttack = 0;
		}
	}
	
	if (this->specialShootUp.show)
	{
		Nokia5110_PrintBMP(this->specialShootUp.posX, this->specialShootUp.posY, 
							this->specialShootUp.image[this->specialShootUp.curStatus], 0);
		if (intCounter%2)
		{
			this->specialShootUp.posX++;
			this->specialShootUp.posY--;
		}
		if (intCounter%6 == 0)
		{
			this->specialShootUp.curStatus = (this->specialShootUp.curStatus + 1)&0x01;
		}
		if ((this->specialShootUp.posX > SCREENW - SPECIAL_SHOOT_SIDE_W) || 
				(this->specialShootUp.posY < SPECIAL_SHOOT_SIDE_H))
		{
			this->specialShootUp.show = false;
		}
	}
	
	if (this->specialShootDown.show)
	{
		Nokia5110_PrintBMP(this->specialShootDown.posX, this->specialShootDown.posY, 
							this->specialShootDown.image[this->specialShootDown.curStatus], 0);
		if (intCounter%2)
		{
			this->specialShootDown.posX++;
			this->specialShootDown.posY++;
		}
		if (intCounter%6 == 0)
		{
			this->specialShootDown.curStatus = (this->specialShootDown.curStatus + 1)&0x01;
		}
		if ((this->specialShootDown.posX > SCREENW - SPECIAL_SHOOT_SIDE_W) || 
				(this->specialShootDown.posY > maxY))
		{
			this->specialShootDown.show = false;
		}
	}
}


//**********************PlayerShip_IncreaseScore***********************
// This function increases the score of the player based on the killed status
// of five enemies.
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 enm1killed: Enemy 1 was killed
//				 enm2killed: Enemy 2 was killed
//				 enm3killed: Enemy 3 was killed
//				 enm4killed: Enemy 4 was killed
//				 enm5killed: Enemy 5 was killed
// outputs: none
void PlayerShip_IncreaseScore(PlayerShip* this, bool enm1killed, bool enm2killed, 
															bool enm3killed, bool enm4killed, bool enm5killed)
{
	this->score += enm1killed + enm2killed + enm3killed + enm4killed + enm5killed;
}


//**********************PlayerShip_isDead***********************
// This function return 1 if the player was killed, else it returns 0
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
bool PlayerShip_isDead(PlayerShip* this)
{
	if (this->dead == 2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//**********************PlayerShip_hasLives***********************
// This function return 1 if the player has lives and therefore can still play, else it returns 0
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
bool PlayerShip_hasLives(PlayerShip* this)
{
	if (this->healthPoints)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//**********************PlayerShip_Reset***********************
// This function revives the player ship 
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
void PlayerShip_Reset(PlayerShip* this)
{
	this->dead = false;
	this->curStatus = 0;
}

