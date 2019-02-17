
#include "GameEngineCycle.h"
#include "Pixel.h"
#include "Enemy.h"
#include "Terrain.h"
#include "../../tm4c123gh6pm.h"
#include "../controls/SlidePot.h"
#include "../controls/Switches.h"
#include "../display/Nokia5110.h"
#include "../display/ImageArrays.h"
#include "../sounds/Sound.h"
#include "../main/Random.h"

// ************************************************************************************************
// ******************************** DEFINITION OF GLOBAL QUANTITIES *******************************
// ************************************************************************************************

#define MAXHP 4
#define MAXGROUND 41  //The ground can be drawn maximal in this Y position

// ************************************************************************************************
// ******************************** DECLARATION OF PRIVATE DATA TYPES *****************************
// ************************************************************************************************

// *************** Structure used for the ship ***************************
typedef struct ShipVariables
{
	unsigned char posX; 	//Position X of the ship
	unsigned char posY; 	//Position Y of the ship
	PixelType shoots[5];	//Array of shoots of the ship
	unsigned char shCounter; //Counter of shoots
	unsigned char healthPoints; //Health points of the ship
	unsigned short score;	//Score of the ship
	bool dead;
	unsigned char lives;
}ShipType;

// **************** Structure used for the animations ********************
typedef struct AnimationVariables
{
	unsigned char *playerShipDestruction[3];			//Array of pointers to the individual image arrays
	unsigned char playerShipDestructionCounter;		//Counter of the animation (used to select which image of the array
}AnimationType;

// ************************************************************************************************
// ******************************** DECLARATION OF GLOBAL VARIABLES *******************************
// ************************************************************************************************

//******************** GLOBAL VARIABLES *****************************
bool Switch_shoot;		//The switch for shooting was pressed
bool Switch_special;  //The switch for special attack was pressed
extern bool Flag;			//Flag sent to main task when the display should be drawn
unsigned char PixelY;	//This value has the position Y of the ship
unsigned long interruptCounter; // It counts how many sysTick interrupts have been occured

ShipType playerShip; 				//Object used to represent the ship
Terrain terrain;				//Object used to represent the terrain
AnimationType animations;		//Object used to represent the animations of the ship
Enemy enemy[5];							//Object used to represent the enemies


// ************************************************************************************************
// ******************************** DECLARATION OF PRIVATE FUNCTIONS ******************************
// ************************************************************************************************

//********************_GroundNextY*********************
// Determines the Y coordinate of the next ground pixel  
// If the last pixel is in downest border of permitted pixels to draw the ground
// The next pixel can only be at the same Y coordinate or 1 above
// If the last pixel is in the uppest border of permitted pixels to draw the ground
// The next pixel can only be at the same Y coordinate or 1 below
// Else the next pixel can be 1 pixel below, 1 pixel above or at the same height
// inputs: PosYLast    The Y coordinate of the last ground pixel drawn, which we use to determine
//                     the Y coordinate of this pixel
//                     0 is on the left; 82 is near the right
//         minY      	 Minimum permitted Y coordinate, it is variable because it changes in the game 
//                     to make it more difficult
// outputs: Coordinate Y of the ground pixel we want to draw now
unsigned char _GroundNextY(unsigned char PosYLast, unsigned char minY);


//**********************_ShowHUD***********************
// Shows the HP and the score of the player at the bottom of the screen
// inputs: none
// outputs: none
void _ShowHUD(void);		

//**********************_ShowBackground***********************
// Generates and show stars at the background
// inputs: none
// outputs: none
void _ShowBackground(void);	

//**********************_ControlShip***********************
// This function do the following tasks:
// - Draws the ship and moves it on dependance of the slide pot
// - Generate the shoots of the ship on dependance of the shoot switch
// inputs: none
// outputs: none
void _ControlShip(void);




// ************************************************************************************************
// *********************************** DEFINITION OF FUNCTIONS ************************************
// ************************************************************************************************

// Initialize SysTick interrupts to trigger at 30 Hz, 33,33 ms
void SysTick_Init(unsigned long period)
{
	NVIC_ST_CTRL_R = 0;					// Disables SysTick timer during configuring
	NVIC_ST_RELOAD_R = period;	// The value is set thinking at the following: 80 MHz / 40 Hz = 2000000
	NVIC_ST_CURRENT_R = 0;			// Clears the current value by writing in it any number
	NVIC_ST_CTRL_R = 0x07;			// Clock source is the precision internal oscilator, interrupts are enabled and the timer is enabled
}

// executes the game engine every 33,3 ms
void SysTick_Handler(void)
{
	Nokia5110_ClearBuffer();
	if (playerShip.dead && animations.playerShipDestructionCounter == 0)
	{
		if (playerShip.healthPoints)
		{
				Nokia5110_OutString_4x4pix_toBuffer(10, 15, "You were hurt");
				Nokia5110_OutString_4x4pix_toBuffer(15, 25, "Press shoot");
				Nokia5110_OutString_4x4pix_toBuffer(10, 30, "to try again!");
				if (Switch_shoot)
				{
					playerShip.dead = false;
				}
		}
		else 
		{
			Nokia5110_OutString_4x4pix_toBuffer(10, 20, "GAME OVER :(");
		}
	}
	else 
	{
		Terrain_ShowTerrain(&terrain, interruptCounter, MAXGROUND);
		_ControlShip();
		Enemy_ControlEnemy(&enemy[0],interruptCounter, MAXGROUND);
		Enemy_ControlEnemy(&enemy[1],interruptCounter, MAXGROUND);
		Enemy_ControlEnemy(&enemy[2],interruptCounter, MAXGROUND);
	}
	_ShowHUD();
	Flag = true;										// Sets the flag to 1, indicating that there is a new sample for the display
	interruptCounter++;
}


//**********************GameEngine_Init***********************
// Calls the initialization of the SysTick timer and give
// initial values to some of the game engine variables
// It needs to be called by the main function of the program
// inputs: none
// outputs: none
void GameEngine_Init(void)
{
	int i;
	SysTick_Init(2666665);	//It initializes the SysTick for 30 Hz
	for (i=0; i<5; i++)
	{
		playerShip.shoots[i].PosX = SHIPW;	// It makes the position of the shoots to be at Y=0 and
		playerShip.shoots[i].PosY = 0;			// X = the width of the ship
	}
	playerShip.shCounter = 0;			// We set the quantity of shown shoots to be 0
	playerShip.healthPoints = 3;	// Initial HP of the ship
	playerShip.score = 0;					// Initial score of the ship
	
	Terrain_InitTerrain(&terrain, MAXGROUND);
	
	animations.playerShipDestruction[0] = (unsigned char*)&PlayerShipDestruction1;  // We point the animation arrays to the individual images on "ImageArrays.h"
	animations.playerShipDestruction[1] = (unsigned char*)&PlayerShipDestruction2;	// So the element 0 of the array points to the first image, the element 1 to
	animations.playerShipDestruction[2] = (unsigned char*)&PlayerShipDestruction3;	// the second image and the element 3 to the third image.
	animations.playerShipDestructionCounter = 0;
	
	for (i=0; i<5; i++)
	{
		Enemy_StartEnemy(&enemy[i], enemy1Alive1, enemy1Alive2, enemy1Alive3, enemy1Dying1, enemy1Dying2);
	}
}

//**********************_ShowHUD***********************
// Shows the HP and the score of the player at the bottom of the screen
// inputs: none
// outputs: none
void _ShowHUD(void)
{
	unsigned char i;
	for (i = 0; i < SCREENW; i++)			// Draws a line at the bottom to separate the HUD from the playing area
	{
			Nokia5110_SetPixel(i, SCREENH - 7);
	}
	Nokia5110_OutString_4x4pix_toBuffer(0, SCREENH - 5, "HP:");
	Nokia5110_OutUDec_4x4pix_toBuffer(15, SCREENH - 5, playerShip.healthPoints);
	Nokia5110_OutChar_4x4pix_toBuffer(20, SCREENH - 5, '/');
	Nokia5110_OutUDec_4x4pix_toBuffer(25, SCREENH - 5, MAXHP);
	Nokia5110_OutString_4x4pix_toBuffer(35, SCREENH - 5, "score:");
	Nokia5110_OutUDec_4x4pix_toBuffer(65, SCREENH - 5, playerShip.score);
}



//**********************_ControlShip***********************
// This function do the following tasks:
// - Draws the ship and moves it on dependance of the slide pot
// - Generate the shoots of the ship on dependance of the shoot switch
// inputs: none
// outputs: none
void _ControlShip(void)
{
	int i;
	//%%%%%%%%%%%%% MOVEMENT OF THE SHIP %%%%%%%%%%%%%%%%% 
	PixelY = SlidePot_toPixelY(SHIPH);							// Converts the ADC data into readable distance value and then to a position in the Y axis
	if (Nokia5110_AskPixel(8,PixelY-(SHIPH/2)) ||	 	// If the pixel for the frontal point of the ship is already set
			Nokia5110_AskPixel(3,PixelY) ||							// or if pixel for the inferior peak is already set 
			Nokia5110_AskPixel(3,PixelY-SHIPH))					// or if pixel for the superior peak is already set
	{
		// We print the elements of the destruction animation
		Nokia5110_PrintBMP(0, PixelY, animations.playerShipDestruction[animations.playerShipDestructionCounter], 0);
		if (interruptCounter%20==0)
		{
			animations.playerShipDestructionCounter++;	// If interrupt
		}
		if (animations.playerShipDestructionCounter == 3)
		{
			Sound_Explosion();
			playerShip.healthPoints--;
			animations.playerShipDestructionCounter = 0;
			playerShip.dead = true;
		}
		return;
	}
	Nokia5110_PrintBMP(0, PixelY, PlayerShipNew, 0);  		// Draws the ship in the display using the value from the slide pot
	
	//%%%%%%%%%%%%%%%%%% NORMAL SHOOTS %%%%%%%%%%%%%%%%%%%
	if (Switch_shoot)																				// If the switch is pressed we do the following operations on the next shoot on the array
	{
		playerShip.shoots[playerShip.shCounter].show = true;							// We make showingShoot true, this variable is used to show the shoot in the display until it dissapears
		Sound_Shoot();																															// We play the shoot sound by pressing the switch
		playerShip.shoots[playerShip.shCounter].PosY = PixelY - (SHIPH/2);  // We set the position Y of the shoot equals to the center of the ship
		playerShip.shoots[playerShip.shCounter].PosX = SHIPW;								// We set the position X of the shoot equals to the pick of the ship
		playerShip.shCounter++;
		if (playerShip.shCounter >= 5) playerShip.shCounter = 0;							// If ShootCounter is bigger or equal to 5, it is setted again to 0
		Switch_shoot = false;																	// We set back the value of the switch
	}
	for (i=0; i<5; i++)						// For every possible shoot
	{
		if (playerShip.shoots[i].show)		// If the shoot should be shown
		{
			Nokia5110_SetPixel(playerShip.shoots[i].PosX, playerShip.shoots[i].PosY);				// We print three pixels which move until they go out of the screen
			Nokia5110_SetPixel(playerShip.shoots[i].PosX + 1, playerShip.shoots[i].PosY);	
			Nokia5110_SetPixel(playerShip.shoots[i].PosX + 2, playerShip.shoots[i].PosY);
			playerShip.shoots[i].PosX++;																		
			if (playerShip.shoots[i].PosX >= SCREENW)
				playerShip.shoots[i].show = false;													// if the shoot reaches the end of the screen, we turn off the value indicating that a shoot should be shown
		}
	}
}

