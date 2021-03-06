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
// Initializes the generation of terrain
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 maxY: indicates the maximal Y coordinate the ground can have
// outputs: none
void Terrain_InitTerrain (Terrain* this, unsigned char maxY);

//**********************Terrain_Create***********************
// Generates and show the terrain on the screen
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 intCounter: Indicates how many cycles of the game engine have occurred
//				 maxY: indicates the maximal Y coordinate the ground can have
// outputs: none
void Terrain_Create(Terrain* this, unsigned long intCounter, unsigned char maxY);

//**********************Terrain_Draw***********************
// Draws the terrain from the top of the hill to the game HUD (determined by maxY)
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 maxY: indicates the maximal Y coordinate the ground can have
// outputs: none
void Terrain_Draw(Terrain* this, unsigned char maxY);

//********************Terrain_GroundNextY*********************
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
//         maxY      	 Maximum permitted Y coordinate
// outputs: Coordinate Y of the ground pixel we want to draw now
unsigned char Terrain_GroundNextY(unsigned char PosYLast, unsigned char minY, unsigned char maxY);

//**********************Terrain_SetHeight***********************
// Sets the height of the terrain
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the terrain
//				 height: indicates the new height of the ground
// outputs: none
void Terrain_SetHeight(Terrain* this, unsigned char height);

#endif
