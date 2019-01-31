

#include "..//tm4c123gh6pm.h"
#include "SlidePot.h"
#include "GameEngine.h"
#include "SlidePot.h"
#include "Nokia5110.h"
#include "ImageArrays.h"
#include "Switches.h"
#include "Sound.h"

unsigned char PosXShoot;
unsigned char PosYShoot;
bool showingShoot;

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
	SlidePot_Init();
	SysTick_Init(2666665);
	PosXShoot = SHIPW;
	PosYShoot = 0;
}

void _ControlShip(void)
{
	PixelY = SlidePot_toPixelY(SHIPH);								// Converts the ADC data into readable distance value
	Nokia5110_PrintBMP(0, PixelY, PlayerShipNew, 0);  // Draws the ship in the display using the value from the slide pot
	if (Switch_shoot)																	// If the switch is pressed
	{
		showingShoot = true;														// We make showingShoot true, this variable is used to show the shoot in the display until it dissapears
		Sound_Shoot();																	// We play the shoot sound by pressing the switch
		PosYShoot = PixelY - (SHIPH/2);									// We set the position Y of the shoot equals to the center of the ship
		PosXShoot = SHIPW;															// We set the position X of the shoot equals to the pick of the ship
		Switch_shoot = false;														// We set back the value of the switch
	}
	if (showingShoot)
	{
		Nokia5110_SetPixel(PosXShoot, PosYShoot);				// We print three pixels which move until they go out of the screen
		Nokia5110_SetPixel(PosXShoot + 1, PosYShoot);	
		Nokia5110_SetPixel(PosXShoot + 2, PosYShoot);
		PosXShoot++;																		
		if (PosXShoot >= SCREENW)
			showingShoot = false;													// if the shoot reaches the end of the screen, we turn off the value indicating that a shoot should be shown
	}
}


// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void)
{
	Nokia5110_ClearBuffer();
	_ControlShip();
	Flag = true;										// Sets the flag to 1, indicating that there is a new sample
}

