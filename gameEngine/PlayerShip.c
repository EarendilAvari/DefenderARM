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

//**********************PlayerShip_ControlShip***********************
// This function do the following tasks:
// - Draws the ship and moves it on dependance of the slide pot
// - Generate the shoots of the ship on dependance of the shoot switch
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 intCounter: Indicates how many cycles of the game engine have occurred
// outputs: none
void PlayerShip_ControlShip(PlayerShip* this, unsigned char intCounter)
{
	unsigned char i;
	unsigned char PixelY;
	static unsigned char PixelY_last;
	bool MovedUp = false;
	bool MovedDown = false;
	bool Hurt = false;
	unsigned char input = 0;
	unsigned char oldStatus = this->curStatus;
	//%%%%%%%%%%%%% MOVEMENT OF THE SHIP %%%%%%%%%%%%%%%%% 
	PixelY = SlidePot_toPixelY(SHIPH);							// Converts the ADC data into readable distance value and then to a position in the Y axis
	if (Nokia5110_AskPixel(8,PixelY-(SHIPH/2)) ||	 	// If the pixel for the frontal point of the ship is already set
			Nokia5110_AskPixel(3,PixelY) ||							// or if pixel for the inferior peak is already set 
			Nokia5110_AskPixel(3,PixelY-SHIPH))					// or if pixel for the superior peak is already set
	{
		Hurt = true;
	}
	
	if (PixelY > PixelY_last)
	{
		MovedDown = true;
	}
	
	if (PixelY < PixelY_last)
	{
		MovedUp = true;
	}
	
	PixelY_last = PixelY;
	
	input = Hurt + (MovedUp<<1) + (MovedDown<<2);
	
	if (intCounter%PlayerShipFSM[oldStatus].delay == 0)
	{
		this->curStatus = PlayerShipFSM[oldStatus].next[input];
	}
	
	if (this->curStatus == shipFSM_Dead)
	{
		this->dead = 2;
		this->healthPoints--;
		return;
	}
	
	Nokia5110_PrintBMP(0, PixelY, this->image[this->curStatus], 0);  		// Draws the ship in the display using the value from the slide pot
	
	//%%%%%%%%%%%%%%%%%% NORMAL SHOOTS %%%%%%%%%%%%%%%%%%%
	if (Switch_shoot)																				// If the switch is pressed we do the following operations on the next shoot on the array
	{
		this->shoots[this->shCounter].show = true;							// We make showingShoot true, this variable is used to show the shoot in the display until it dissapears
		Sound_Shoot();																															// We play the shoot sound by pressing the switch
		this->shoots[this->shCounter].PosY = PixelY - (SHIPH/2);  // We set the position Y of the shoot equals to the center of the ship
		this->shoots[this->shCounter].PosX = SHIPW;								// We set the position X of the shoot equals to the pick of the ship
		this->shCounter++;
		if (this->shCounter >= 5) this->shCounter = 0;							// If ShootCounter is bigger or equal to 5, it is setted again to 0
		Switch_shoot = false;																	// We set back the value of the switch
	}
	for (i=0; i<5; i++)						// For every possible shoot
	{
		if (this->shoots[i].show)		// If the shoot should be shown
		{
			Nokia5110_SetPixel(this->shoots[i].PosX, this->shoots[i].PosY);				// We print three pixels which move until they go out of the screen
			Nokia5110_SetPixel(this->shoots[i].PosX + 1, this->shoots[i].PosY);	
			Nokia5110_SetPixel(this->shoots[i].PosX + 2, this->shoots[i].PosY);
			this->shoots[i].PosX++;																		
			if (this->shoots[i].PosX >= SCREENW)
				this->shoots[i].show = false;													// if the shoot reaches the end of the screen, we turn off the value indicating that a shoot should be shown
		}
	}
}

