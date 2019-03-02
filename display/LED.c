

// Implementation of two LEDs on PB4 and PB5

#include "../../tm4c123gh6pm.h"
#include "LED.h"

// **************LED_Init*********************
// Initialize 2 LEDs to measure times on the game
// Input: none
// Output: none
void LED_Init(void)
{
	unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000002;	// Enables clock for Port B
	delay = SYSCTL_RCGC2_R;
	
	GPIO_PORTB_DIR_R |= 0x30;		// Makes PB4 and PB5 outputs
	GPIO_PORTB_AMSEL_R &= ~0x30;	// Turns OFF analog functions on PB4 and PB5
	GPIO_PORTB_AFSEL_R &= ~0x30;	// Turns OFF alternate functions on PB4 and PB5
	GPIO_PORTB_PCTL_R &= ~0x00FF0000;	// Unselects any alternate function on PB4 and PB5
	GPIO_PORTB_DEN_R |= 0x30;			// Enables digital function on PB4 and PB5
}

// **************LED_SetGreen*********************
// Turns on green LED
// Input: none
// Output: none
void LED_SetGreen(void)
{
	GPIO_PORTB_DATA_R |= 0x10;
}

// **************LED_ResetGreen*********************
// Turns off green LED
// Input: none
// Output: none
void LED_ResetGreen(void)
{
	GPIO_PORTB_DATA_R &= ~0x10;
}

// **************LED_ToggleGreen*********************
// Toggles green LED
// Input: none
// Output: none
void LED_ToggleGreen(void)
{
	GPIO_PORTB_DATA_R ^= 0x10;
}


// **************LED_SetYellow*********************
// Turns on yellow LED
// Input: none
// Output: none
void LED_SetYellow(void)
{
	GPIO_PORTB_DATA_R |= 0x20;
}

// **************LED_ResetYellow*********************
// Turns off yellow LED
// Input: none
// Output: none
void LED_ResetYellow(void)
{
	GPIO_PORTB_DATA_R &= ~0x20;
}

