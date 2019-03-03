// DAC.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// Implementation of the 4-bit digital to analog converter
// Daniel Valvano, Jonathan Valvano
// December 29, 2014
// Port B bits 3-0 have the 4-bit DAC

#include "../../tm4c123gh6pm.h"
#include "DAC.h"


// **************DAC_Init*********************
// Initialize 4-bit DAC using PB0-5
// Input: none
// Output: none
void DAC_Init(void)
{
	unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000002; // Activates clock for port B
	delay = SYSCTL_RCGC2_R;			 // Waits to stabilize the clock of port B
	
	GPIO_PORTB_DIR_R |= 0x0F;		  // Makes PB0-3 outputs
	GPIO_PORTB_AMSEL_R &= ~0x0F;	// Turn OFF analog functions for PB0-3
	GPIO_PORTB_AFSEL_R &= ~0x0F;	// Turns OFF alternate functions for PB0-3
	GPIO_PORTB_PCTL_R &= ~0x0000FFFF;		// Unselects any alternate function for PB0-3
	GPIO_PORTB_DR8R_R |= 0x0F;
	GPIO_PORTB_DEN_R |= 0x0F;						// Activates digital function for PB0-3
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data)
{
	unsigned long out = data&0x0F;
  GPIO_PORTB_DATA_R &= ~0x0F;		// Sends the data to the DAC pins (port B).
	GPIO_PORTB_DATA_R |= out;
																		// It masks the bits in order to be friendly and don't change the other
																		// bits of the port
}
