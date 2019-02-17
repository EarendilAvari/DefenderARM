#ifndef PIXEL_H
#define PIXEL_H

#include <stdbool.h>

// ************************************************************************************************
// *********************************** DECLARATION OF "CLASS" *************************************
// ************************************************************************************************

// ************** Structure used for the normal shoots *****************
typedef struct Pixel 	// A structure for elements represented as a pixel
{
	unsigned char PosX;	// Position X of that pixel
	unsigned char PosY; // Position Y of that pixel
	bool show;					// True if the pixel should be shown
}PixelType;

#endif
