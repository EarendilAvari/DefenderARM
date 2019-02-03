
#include "..//tm4c123gh6pm.h"
#include "SlidePot.h"
#include "GameEngine.h"
#include "SlidePot.h"
#include "Nokia5110.h"
#include "ImageArrays.h"
#include "Switches.h"
#include "Sound.h"
#include "Random.h"

#define MAXHP 4

bool Switch_shoot;
bool Switch_special;

// Structure used for the normal shoots
typedef struct Pixel
{
	unsigned char PosX;
	unsigned char PosY;
	bool show;
}PixelType;

// Structure used to define the terrain
typedef struct TerrainVariables
{
	PixelType backgroundStars[50];	
	PixelType ground[SCREENW];
	unsigned char starCounter;
	unsigned char groundCounter;
}TerrainType;

// Structure used for the ship
typedef struct ShipVariables
{
	unsigned char posX;
	unsigned char posY;
	PixelType shoots[5];
	unsigned char shCounter;
	unsigned char healthPoints;
	unsigned short score;
	bool dead;
	unsigned char lives;
}ShipType;

ShipType playerShip;
TerrainType terrain;
unsigned long interruptCounter; // It counts how many sysTick interrupts have been occured

// Initialize SysTick interrupts to trigger at 30 Hz, 25 ms
void SysTick_Init(unsigned long period)
{
	NVIC_ST_CTRL_R = 0;					// Disables SysTick timer during configuring
	NVIC_ST_RELOAD_R = period;	// The value is set thinking at the following: 80 MHz / 40 Hz = 2000000
	NVIC_ST_CURRENT_R = 0;			// Clears the current value by writing in it any number
	NVIC_ST_CTRL_R = 0x07;			// Clock source is the precision internal oscilator, interrupts are enabled and the timer is enabled
}

void GameEngine_Init(void)
{
	int i;
	SlidePot_Init();
	SysTick_Init(2666665);
	for (i=0; i<5; i++)
	{
		playerShip.shoots[i].PosX = SHIPW;
		playerShip.shoots[i].PosY = 0;
	}
	for (i=0; i<50; i++)
	{
		terrain.backgroundStars[i].PosX = 0;
		terrain.backgroundStars[i].PosY = 0;
		terrain.backgroundStars[i].show = false;
	}
	for (i=0; i<SCREENW; i++)
	{
		terrain.ground[i].PosX = i;
		if (i==0)
		{	
			terrain.ground[i].PosY = 20 + Random32()%(SCREENH - 27);
		}
		else
		{
			terrain.ground[i].PosY = (terrain.ground[i-1].PosY - 1) + Random32()%3;
		}
		terrain.ground[i].show = true;
	}
	terrain.groundCounter = SCREENW;
	playerShip.shCounter = 0;
	playerShip.healthPoints = 3;
	playerShip.score = 0;
}


void _ShowHUD(void)
{
	unsigned char i;
	for (i = 0; i < SCREENW; i++)
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

void _ShowBackground(void)
{
	unsigned char i;
	if ((interruptCounter%5) == Random32()%5) // If the modulo of interruptCounter%5 is equal to a random number between 1 and 4
	{
		terrain.backgroundStars[terrain.starCounter].show = true; 							//We show a star
		terrain.backgroundStars[terrain.starCounter].PosX = SCREENW - 1;				//With x start posision at the end of the screen
		terrain.backgroundStars[terrain.starCounter].PosY = Random32()%(SCREENH-7);	//And y start position a number between 0 and SCREENH - 8
		terrain.starCounter++;
		if (terrain.starCounter >= 50) terrain.starCounter = 0;									//We make place for 50 stars, if a 51 star is created, the star 0 is overwritten
	}
	for (i=0; i<50; i++)																											//We do this for every star
	{
		if ((terrain.backgroundStars[i].show) && ((interruptCounter%2)==0))			//If the star is meant to be shown and the interruptCounter is even 
		{
			Nokia5110_SetPixel(terrain.backgroundStars[i].PosX, terrain.backgroundStars[i].PosY); 		// We print two pixels representing the stars
			Nokia5110_SetPixel(terrain.backgroundStars[i].PosX - 1, terrain.backgroundStars[i].PosY);
			terrain.backgroundStars[i].PosX--;																		// We move the positionX a pixel behind
			if (terrain.backgroundStars[i].PosX < 1)															// If the star reached the end of the display we dont show it anymore
				terrain.backgroundStars[i].show = false;
		}
	}
}

void _ShowTerrain(void)
{
	unsigned char i;
	if (interruptCounter%2)
	{
		terrain.ground[terrain.groundCounter].show = true;
		terrain.ground[terrain.groundCounter].PosX = SCREENW - 1;
		if (terrain.groundCounter > 0)
		{
			terrain.ground[terrain.groundCounter].PosY = 
			(terrain.ground[terrain.groundCounter - 1].PosY - 1) + Random32()%3;
		}
		else 
		{
			terrain.ground[terrain.groundCounter].PosY =
			(terrain.ground[SCREENW].PosY - 1) + Random32()%3;
		}
		terrain.groundCounter++;
		if (terrain.groundCounter >= SCREENW) terrain.groundCounter = 0;
	}
	for (i=0; i<SCREENW; i++)
	{
		if ((terrain.ground[i].show) && ((interruptCounter%2)==0))
		{
			Nokia5110_SetPixel(terrain.ground[i].PosX, terrain.ground[i].PosY);
			terrain.ground[i].PosX--;
			if (terrain.ground[i].PosX < 1)
				terrain.ground[i].show = false;
		}
	}
}

void _ControlShip(void)
{
	int i;
	//%%%%%%%%%%%%% MOVEMENT OF THE SHIP %%%%%%%%%%%%%%%%% 
	PixelY = SlidePot_toPixelY(SHIPH);										// Converts the ADC data into readable distance value and then to a position in the Y axis
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


// executes the game engine every 33,3 ms
void SysTick_Handler(void)
{
	Nokia5110_ClearBuffer();
	_ShowBackground();
	_ShowTerrain();
	_ShowHUD();
	_ControlShip();
	interruptCounter++;
	Flag = true;										// Sets the flag to 1, indicating that there is a new sample for the display
}

