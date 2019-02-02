

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
typedef struct ShootVariables
{
	unsigned char PosX;
	unsigned char PosY;
	bool showShoot;
}ShootType;

typedef struct ShipVariables
{
	unsigned char posX;
	unsigned char posY;
	ShootType shoots[5];
	unsigned char shCounter;
	unsigned char healthPoints;
	unsigned short score;
	bool dead;
	unsigned char lives;
}ShipType;

ShipType playerShip;

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
		playerShip.shoots[playerShip.shCounter].showShoot = true;							// We make showingShoot true, this variable is used to show the shoot in the display until it dissapears
		Sound_Shoot();																															// We play the shoot sound by pressing the switch
		playerShip.shoots[playerShip.shCounter].PosY = PixelY - (SHIPH/2);  // We set the position Y of the shoot equals to the center of the ship
		playerShip.shoots[playerShip.shCounter].PosX = SHIPW;								// We set the position X of the shoot equals to the pick of the ship
		playerShip.shCounter++;
		if (playerShip.shCounter >= 5) playerShip.shCounter = 0;							// If ShootCounter is bigger or equal to 5, it is setted again to 0
		Switch_shoot = false;																	// We set back the value of the switch
	}
	for (i=0; i<5; i++)						// For every possible shoot
	{
		if (playerShip.shoots[i].showShoot)		// If the shoot should be shown
		{
			Nokia5110_SetPixel(playerShip.shoots[i].PosX, playerShip.shoots[i].PosY);				// We print three pixels which move until they go out of the screen
			Nokia5110_SetPixel(playerShip.shoots[i].PosX + 1, playerShip.shoots[i].PosY);	
			Nokia5110_SetPixel(playerShip.shoots[i].PosX + 2, playerShip.shoots[i].PosY);
			playerShip.shoots[i].PosX++;																		
			if (playerShip.shoots[i].PosX >= SCREENW)
				playerShip.shoots[i].showShoot = false;													// if the shoot reaches the end of the screen, we turn off the value indicating that a shoot should be shown
		}
	}
}


// executes the game engine every 33,3 ms
void SysTick_Handler(void)
{
	Nokia5110_ClearBuffer();
	_ShowHUD();
	_ControlShip();
	Flag = true;										// Sets the flag to 1, indicating that there is a new sample for the display
}

