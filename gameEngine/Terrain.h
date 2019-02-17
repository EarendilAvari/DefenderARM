#ifndef TERRAIN_H
#define TERRAIN_H

// ************************************************************************************************
// ******************************* INTERACTION WITH ANOTHER MODULES *******************************
// ************************************************************************************************
#include "Pixel.h"
#include "../Main/Random.h"
#include "../Display/Nokia5110.h"

// ************************************************************************************************
// *********************************** DECLARATION OF "CLASS" *************************************
// ************************************************************************************************

// ************** Structure used to define the terrain ******************
typedef struct TerrainVariables
{
	PixelType ground[SCREENW];      //Pixels that symbolize the ground
	unsigned char groundCounter;
	unsigned char minGroundH;
}Terrain;

// ************************************************************************************************
// ******************************** DECLARATION OF PUBLIC FUNCTIONS *******************************
// ************************************************************************************************

//**********************Terrain_InitTerrain***********************
// Generates and show the terrain on the screen
// inputs: none
// outputs: none
void Terrain_InitTerrain (Terrain* this, unsigned char maxY);

//**********************Terrain_ShowTerrain***********************
// Generates and show the terrain on the screen
// inputs: none
// outputs: none
void Terrain_ShowTerrain(Terrain* this, unsigned long intCounter, unsigned char maxY);

//********************_GroundNextY*********************
// Determines the Y coordinate of the next ground pixel  
// If the last pixel is in downest border of permitted pixels to draw the ground
// The next pixel can only be at the same Y coordinate or 1 above
// If the last pixel is in the uppest border of permitted pixels to draw the ground
// The next pixel can only be at the same Y coordinate or 1 below
// Else the next pixel can be 1 pixel below, 1 pixel above or at the same height
// inputs: PosYLast    The Y coordinate of the last ground pixel drawn, which we use to determine
//                     the Y coordinate of this pixel
//                     0 is on the left; 82 is near the right
//         minY      	 Minimum permitted Y coordinate, it is variable because it changes in the game 
//                     to make it more difficult
// outputs: Coordinate Y of the ground pixel we want to draw now
unsigned char Terrain_GroundNextY(unsigned char PosYLast, unsigned char minY, unsigned char maxY);

#endif
