

#include "ADC.h"

void SlidePot_Init(void)
{
	ADC0_Init();
}

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.
// Overflow and dropout should be considered
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long SlidePot_Convert(unsigned long sample)
{
	// Completely linear version
	//float yMax = 6000;
	//float yMin = 1;
	//float xMax = 4095;
	//float xMin = 0;

	//float m = (yMax - yMin)/(xMax - xMin);	// Slope of the linear equation

	//float outConverted = m*((float)sample - xMin) + yMin;

	// return (unsigned long)outConverted;

	// Calibrated version

	long A, B, outConverted;

	// Constants A and B are calculated by taking diverse points in the form (Distance, ADCValue)
	// I calculated the slopes between the different points and I calculated the average to get A
	// I calculated the offsets in the form Distance - Slope*ADCValue, then I calculated the average to get B

	if (sample < 200)
	{
		A = 2960;
		B = 0;
	}
	else if (sample > 4000)
	{
		A = 4818;
		B = -13291;
	}
	else
	{
		A = 1341;
		B = 296;
	}

	outConverted = ((A*sample)>>10) + B;

  return outConverted;
}

unsigned char SlidePot_toPixelY(unsigned char height)
{
	unsigned long dist = SlidePot_Convert(ADC0_In());
	unsigned long PixelY;
	
	if (dist < 1000) 
	{
		PixelY = height;
	}
	else if (dist > 4500)
	{
		PixelY = 40;		// Change after for 48 - SizeY of the ship
	}
	else
	{
		PixelY = ((40 - height)*(dist - 1000))/3500 + height; // Here is important to make sure that the division is done as the last operation
																													// If not, the result may end being zero.
																													// We scale the distance measured to the coordinate Y of the display
																													// using the equation y = ((y1 - y0)/(x1 - x0))*x + y0
																													// where y1 = 40, y0 = height, x1 = 4500, y1 = 1000
	}
	return (unsigned char)PixelY;
}


