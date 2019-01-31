

#include "..//tm4c123gh6pm.h"
#include "SlidePot.h"
#include "GameEngine.h"
#include "SlidePot.h"
#include "Nokia5110.h"
#include "ImageArrays.h"

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
}

void _ControlShip(void)
{
	PixelY = SlidePot_toPixelY(SHIPH);	// Converts the ADC data into readable distance value
	Nokia5110_PrintBMP(0, PixelY, PlayerShipNew, 0);
}


// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void)
{
	Nokia5110_ClearBuffer();
	_ControlShip();
	Flag = true;										// Sets the flag to 1, indicating that there is a new sample
}

