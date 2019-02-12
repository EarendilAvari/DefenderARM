
#include "../../tm4c123gh6pm.h"
#include "Switches.h"

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
	
	GPIO_PORTE_IS_R &= ~0x03;				// (h) PE0 and PE1 are edge sensitives
	GPIO_PORTE_IBE_R &= ~0x03;			// PE0 and PE1 are not both edges
	GPIO_PORTE_IEV_R &= ~0x03;			// The interrupts are caused by a falling edge
	GPIO_PORTE_ICR_R = 0x03;				// (i) Clear flags
	GPIO_PORTE_IM_R |= 0x03;				// (j) Arm interrupts on PE0 and PE1
	
	NVIC_PRI1_R = (NVIC_PRI1_R&0xFFFFFF3F) | 0x000000C0; // (k) Priority 5 
	NVIC_EN0_R |= 0x00000010;				// Enables interrupt 4 in NVIC 
}

void GPIOPortE_Handler(void)
{
	if (GPIO_PORTE_RIS_R&0x01)
	{
		GPIO_PORTE_ICR_R = 0x01;
		Switch_shoot = true;
	}
	else if (GPIO_PORTE_RIS_R&0x02)
	{
		GPIO_PORTE_ICR_R = 0x02;
		Switch_special = true;
	}
}

