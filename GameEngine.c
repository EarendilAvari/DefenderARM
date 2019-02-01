

#include "..//tm4c123gh6pm.h"
#include "SlidePot.h"
#include "GameEngine.h"
#include "SlidePot.h"
#include "Nokia5110.h"
#include "ImageArrays.h"
#include "Switches.h"
#include "Sound.h"
#include "Random.h"

bool Switch_shoot;
bool Switch_special;

// Structure used for the normal shoots
typedef struct ShootVariables
{
	unsigned char PosXShoot;
	unsigned char PosYShoot;
	bool showingShoot;
}ShootType;

typedef struct ShipVariables
{
	unsigned char posX;
	unsigned char posY;
	ShootType shoots[5];
	unsigned char shootCounter;
	unsigned char healthPoints;
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
		playerShip.shoots[i].PosXShoot = SHIPW;
		playerShip.shoots[i].PosYShoot = 0;
	}
	playerShip.shootCounter = 0;
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
		playerShip.shoots[playerShip.shootCounter].showingShoot = true;							// We make showingShoot true, this variable is used to show the shoot in the display until it dissapears
		Sound_Shoot();																															// We play the shoot sound by pressing the switch
		playerShip.shoots[playerShip.shootCounter].PosYShoot = PixelY - (SHIPH/2);  // We set the position Y of the shoot equals to the center of the ship
		playerShip.shoots[playerShip.shootCounter].PosXShoot = SHIPW;								// We set the position X of the shoot equals to the pick of the ship
		playerShip.shootCounter++;
		if (playerShip.shootCounter >= 5) playerShip.shootCounter = 0;							// If ShootCounter is bigger or equal to 5, it is setted again to 0
		Switch_shoot = false;																	// We set back the value of the switch
	}
	for (i=0; i<5; i++)						// For every possible shoot
	{
		if (playerShip.shoots[i].showingShoot)		// If the shoot should be shown
		{
			Nokia5110_SetPixel(playerShip.shoots[i].PosXShoot, playerShip.shoots[i].PosYShoot);				// We print three pixels which move until they go out of the screen
			Nokia5110_SetPixel(playerShip.shoots[i].PosXShoot + 1, playerShip.shoots[i].PosYShoot);	
			Nokia5110_SetPixel(playerShip.shoots[i].PosXShoot + 2, playerShip.shoots[i].PosYShoot);
			playerShip.shoots[i].PosXShoot++;																		
			if (playerShip.shoots[i].PosXShoot >= SCREENW)
				playerShip.shoots[i].showingShoot = false;													// if the shoot reaches the end of the screen, we turn off the value indicating that a shoot should be shown
		}
	}
}


// executes the game engine every 33,3 ms
void SysTick_Handler(void)
{
	Nokia5110_ClearBuffer();
	_ControlShip();
	Flag = true;										// Sets the flag to 1, indicating that there is a new sample for the display
}

