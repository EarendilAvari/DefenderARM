// ************************************************************************************************
// ******************************* INTERACTION WITH ANOTHER MODULES *******************************
// ************************************************************************************************
#include "Terrain.h"

// ************************************************************************************************
// *********************************** DEFINITION OF FUNCTIONS ************************************
// ************************************************************************************************

//**********************Terrain_InitTerrain***********************
// Generates and show the terrain on the screen
// inputs: none
// outputs: none
void Terrain_InitTerrain (Terrain* this, unsigned char maxY)
{
	unsigned char i;
	this->minGroundH = maxY - 10;		// It initializes the upper border of the terrain (ground) to be 10 
																					// pixels above the border down, where the HUD starts
	this->ground[0].PosX = 0;							// It makes the position of the first ground pixel to be 0,MAXGROUND, that means it 
																					// starts down with the HUD
	this->ground[0].PosY = maxY;
	for (i=1; i<SCREENW; i++)								// For every other border pixel, we make the position X to be i, so we fill the 
	{																				// screen with border pixels			
		this->ground[i].PosX = i;						
		this->ground[i].PosY = Terrain_GroundNextY(this->ground[i-1].PosY, this->minGroundH, maxY); // The coordinate Y is random and depends
	}																																											 // of the coordinate Y of the last pixel
	this->groundCounter = 0;		// We set the ground counter to be 0, so the next ground pixel will "steal" the place of the pixel located at X=0
																// Very important that it is 0, else we get a bug
}



//**********************Terrain_ShowTerrain***********************
// Generates and show the terrain on the screen
// inputs: none
// outputs: none
void Terrain_ShowTerrain(Terrain* this, unsigned long intCounter, unsigned char maxY)
{
	unsigned char i, j;
	unsigned char PosYLast;				//Coordinate Y of the ground pixel used to generate the coordinate of the next one
	if (intCounter%5 == 0)	//It is executed every 5 interrupts, so every 166,67 ms, causing a sensation that it moves
																//slower than the shoots
	{
		for (i = 0; i < SCREENW; i++)	//We move the ground pixels 1 coordinate to the left
		{
			this->ground[i].PosX--;
		}
		this->ground[this->groundCounter].PosX = SCREENW - 1;		//We set the coordinate X of the new pixel to be at the rightmost border
		if (this->groundCounter > 0)																//If the pixel we will change now is not the number 0 of the array
		{																															//we use the previous pixel in the array to generate the coordinate Y
			PosYLast = this->ground[this->groundCounter - 1].PosY;	//of the new ground pixel
		}
		else																													//If the pixel we will change now IS THE NUMBER 0 of the array
		{																															//We use the last pixel of the array to generate the coordinate Y
			PosYLast = this->ground[SCREENW - 1].PosY;								//of the new ground pixel
		}
		this->ground[this->groundCounter].PosY = Terrain_GroundNextY(PosYLast, this->minGroundH, maxY);	//This function generates a random value between the previous
																																																			// and the following Y coordinate
		this->groundCounter++;																										//We increase the groundCounter by one, meaning that we will change the next 
																																							//element of the array in the next iteration
		if (this->groundCounter >= SCREENW) this->groundCounter = 0;  			//If the counter reaches the length of the array, we set it back to 0, to process again the 
																																				//first pixel on the array
	}
	for (i=0; i<SCREENW; i++)																//We draw this in every iteration. For every ground pixel we set all the pixels under it creating a 
																													//mountain on the display
	{
		for (j = this->ground[i].PosY; j < maxY; j++)	
		{
			Nokia5110_SetPixel(this->ground[i].PosX, j);
		}
	}
}


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
// outputs: Coordinate Y of the ground pixel we want to draw now
unsigned char Terrain_GroundNextY(unsigned char PosYLast, unsigned char minY, unsigned char maxY)
{
		if (PosYLast + 1 > maxY)
		{
			return (PosYLast - 1) + Random32()%2;
		}
		else if (PosYLast - 1 < minY)
		{
			return (PosYLast + 1) - Random32()%2;
		}				
		else
		{
			return (PosYLast - 1) + Random32()%3;
		}
}
