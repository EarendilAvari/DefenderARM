// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0 SS3 to be triggered by
// software and trigger a conversion, wait for it to finish,
// and return the result.
// Daniel Valvano
// January 15, 2016

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015

 Copyright 2016 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include "ADC.h"
#include "..//tm4c123gh6pm.h"

// This initialization function sets up the ADC
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Init(void)
{
	unsigned long delay;
  SYSCTL_RCGC2_R |= 0x10;					// Turns ON clock for PORT E
	SYSCTL_RCGC0_R |= 0x00010000;		// Turns ON clock for ADC0
	delay = SYSCTL_RCGCGPIO_R;			// Makes an operation to give the clocks time enough to stabilize

	GPIO_PORTE_DIR_R &= ~0x04;			// Makes PE an input
	GPIO_PORTE_AFSEL_R |= 0x04;			// Turns ON alternate function for PE2
	GPIO_PORTE_AMSEL_R |= 0x04;			// Turns ON analog function for PE2
	GPIO_PORTE_DEN_R &= ~0x04;			// Turns OFF digital function for PE2

	ADC0_PC_R |= 0x01;							// The sampling rate is set to a maximum of 125 ksps
	ADC0_SSPRI_R = 0x01234;					// The priorities of the sequencer are set so the sequencer 3 has the highest priority
	ADC0_ACTSS_R &= ~0x08;						// Sequencer 3 is turned OFF for configuration
	ADC0_EMUX_R = (ADC0_EMUX_R&0xFFFF0FFF) + (0<<24);	// The trigger for starting a sampling is set to be manual
																										// The register ADCPSSI needs to be written in order to start
																										// a sampling process.
	ADC0_SSMUX3_R = 1;							// ADC0(PE2) is being used
	ADC0_SSCTL3_R = 0x06;						//Field IE0 and END0 are set, that means that the RIS register is set when a new sample is done
																	// and that this sample is the last of the sequence (and the only one in this case)
	ADC0_IM_R &= ~0x000F000F;				// All interrupts for the ADC0 are turned OFF

	ADC0_ACTSS_R |= 0x08;						// Sequencer 3 is turned ON when the configuration is done

}


//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void)
{
	unsigned long result;
  ADC0_PSSI_R |= 0x00000008;			// Initiates sampling on sequencer 3
	while ((ADC0_RIS_R&0x08) == 0)
	{
		// waits until the sample is done
	}
	result = ADC0_SSFIFO3_R&0x00000FFF;	// Copies the sample from the FIFO of sequencer 3 to result
	ADC0_ISC_R |= 0x00000008;						// clears the RIS register

	return result;											// Returns the result value

}
