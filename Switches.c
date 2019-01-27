
#include "Switches.h"
#include "..//tm4c123gh6pm.h"


// Initialization of switches connected to PE0 and PE1
// using edge interrupt 
void SwitchesInit(void)
{
	unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000010; 	// (a) Activates clock for port E
	delay = SYSCTL_RCGC2_R;					// (b) Waits for clock to stabilize
	GPIO_PORTE_DIR_R &= ~0x03;			// (c) Makes PE0 and PE1 inputs
	GPIO_PORTE_AFSEL_R &= ~0x03;		// (d) Deactivates alternate functions for PE0 and PE1
	GPIO_PORTE_PCTL_R &= ~0x000000FF;	// (e) Configures PE0 and PE1 to GPIO
	GPIO_PORTE_DEN_R |= 0x03;				// (f) Enables digital funtion for PE0 and PE1
	GPIO_PORTE_PDR_R |= 0x03;				// (g) Enables pull down register for both switches
	
	// Edge interrupt to do later
	
}

