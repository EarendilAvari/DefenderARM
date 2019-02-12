#ifndef GAMEENGINEMAIN_H
#define GAMEENGINEMAIN_H

#include <stdbool.h>

#define MAXHP 4
#define MAXGROUND 41  //The ground can be drawn maximal in this Y position


//********** Finite state machine for enemy animation **************
typedef const struct enemyFSM_Variables
{
	unsigned char imgNumber;
	unsigned short delay;
	unsigned char next[2];
}enemyFSM_Type;

#define enemyFSM_Alive1 0
#define enemyFSM_Alive2 1
#define enemyFSM_Alive3 2
#define enemyFSM_Dying1 3
#define enemyFSM_Dying2 4
#define enemyFSM_NoShow 5

typedef struct enemyVariables
{
	unsigned char posX;
	unsigned char posY;
	unsigned char dead;
	unsigned char *image[5];
	unsigned char actStatus;	
}enemyType;



// ************** Structure used for the normal shoots *****************
typedef struct Pixel 	// A structure for elements represented as a pixel
{
	unsigned char PosX;	// Position X of that pixel
	unsigned char PosY; // Position Y of that pixel
	bool show;					// True if the pixel should be shown
}PixelType;

// ************** Structure used to define the terrain ******************
typedef struct TerrainVariables
{
	PixelType backgroundStars[50];	//Background stars to be shown
	PixelType ground[SCREENW];      //Pixels that symbolize the ground
	unsigned char starCounter; 			//How many stars are shown
	unsigned char groundCounter;
	unsigned char minGroundH;
}TerrainType;

// *************** Structure used for the ship ***************************
typedef struct ShipVariables
{
	unsigned char posX; 	//Position X of the ship
	unsigned char posY; 	//Position Y of the ship
	PixelType shoots[5];	//Array of shoots of the ship
	unsigned char shCounter; //Counter of shoots
	unsigned char healthPoints; //Health points of the ship
	unsigned short score;	//Score of the ship
	bool dead;
	unsigned char lives;
}ShipType;

// **************** Structure used for the animations ********************
typedef struct AnimationVariables
{
	unsigned char *playerShipDestruction[3];			//Array of pointers to the individual image arrays
	unsigned char playerShipDestructionCounter;		//Counter of the animation (used to select which image of the array
}AnimationType;


//**********************GameEngine_Init***********************
// Calls the initialization of the SysTick timer and give
// initial values to some of the game engine variables
// It needs to be called by the main function of the program
// inputs: none
// outputs: none
void GameEngine_Init(void);

#endif

