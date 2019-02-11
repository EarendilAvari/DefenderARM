
#include "..//tm4c123gh6pm.h"
#include "SlidePot.h"
#include "GameEngine.h"
#include "SlidePot.h"
#include "Nokia5110.h"
#include "ImageArrays.h"
#include "Switches.h"
#include "Sound.h"
#include "Random.h"

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

enemyFSM_Type enemyFSM[6] =
{
	/* STATE: Alive1 */ {0, 50, {enemyFSM_Alive2, enemyFSM_Dying1}},
	/* STATE: Alive2 */ {1, 10, {enemyFSM_Alive3, enemyFSM_Dying1}},
	/* STATE: Alive3 */ {2, 10, {enemyFSM_Alive1, enemyFSM_Dying1}},
	/* STATE: Dying1 */ {3, 20, {enemyFSM_Dying2, enemyFSM_Dying2}},
	/* STATE: Dying2 */ {4, 20, {enemyFSM_NoShow, enemyFSM_NoShow}},
	/* STATE: NoShow */ {5, 0, {enemyFSM_NoShow, enemyFSM_NoShow}}
};

typedef struct enemyVariables
{
	unsigned char posX;
	unsigned char posY;
	unsigned char dead;
	unsigned char *image[5];
	unsigned char actStatus;	
}enemyType;

enemyType enemy[5];

#define ENEMYW 12
#define ENEMYH 8

bool Switch_shoot;		//The switch for shooting was pressed
bool Switch_special;  //The switch for special attack was pressed

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

ShipType playerShip; 	//Object used to represent the ship
TerrainType terrain;	//Object used to represent the terrain
AnimationType animations;
unsigned long interruptCounter; // It counts how many sysTick interrupts have been occured


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
unsigned char _GroundNextY(unsigned char PosYLast, unsigned char minY);


//**********************_ShowHUD***********************
// Shows the HP and the score of the player at the bottom of the screen
// inputs: none
// outputs: none
void _ShowHUD(void);		

//**********************_ShowBackground***********************
// Generates and show stars at the background
// inputs: none
// outputs: none
void _ShowBackground(void);	

//**********************_ShowTerrain***********************
// Generates and show the terrain on the screen
// inputs: none
// outputs: none
void _ShowTerrain(void);

//**********************_ControlShip***********************
// This function do the following tasks:
// - Draws the ship and moves it on dependance of the slide pot
// - Generate the shoots of the ship on dependance of the shoot switch
// inputs: none
// outputs: none
void _ControlShip(void);

//**********************_ControlEnemy***********************
// This function do the following tasks:
// - Draws the enemy
// - Moves it randomly  
// - Generates its shoots randomly
// inputs: enemyNumber: Number of the array element of enemy
// outputs: none
void _ControlEnemy(unsigned char enemyNmb);

// Initialize SysTick interrupts to trigger at 30 Hz, 33,33 ms
void SysTick_Init(unsigned long period)
{
	NVIC_ST_CTRL_R = 0;					// Disables SysTick timer during configuring
	NVIC_ST_RELOAD_R = period;	// The value is set thinking at the following: 80 MHz / 40 Hz = 2000000
	NVIC_ST_CURRENT_R = 0;			// Clears the current value by writing in it any number
	NVIC_ST_CTRL_R = 0x07;			// Clock source is the precision internal oscilator, interrupts are enabled and the timer is enabled
}

// executes the game engine every 33,3 ms
void SysTick_Handler(void)
{
	Nokia5110_ClearBuffer();
	if (playerShip.dead && animations.playerShipDestructionCounter == 0)
	{
		if (playerShip.healthPoints)
		{
				Nokia5110_OutString_4x4pix_toBuffer(10, 15, "You were hurt");
				Nokia5110_OutString_4x4pix_toBuffer(15, 25, "Press shoot");
				Nokia5110_OutString_4x4pix_toBuffer(10, 30, "to try again!");
				if (Switch_shoot)
				{
					playerShip.dead = false;
				}
		}
		else 
		{
			Nokia5110_OutString_4x4pix_toBuffer(10, 20, "GAME OVER :(");
		}
	}
	else 
	{
		_ShowTerrain();
		_ControlShip();
		_ControlEnemy(0);
		_ControlEnemy(1);
		_ControlEnemy(2);
	}
	_ShowHUD();
	Flag = true;										// Sets the flag to 1, indicating that there is a new sample for the display
	interruptCounter++;
}


//**********************GameEngine_Init***********************
// Calls the initialization of the SysTick timer and give
// initial values to some of the game engine variables
// It needs to be called by the main function of the program
// inputs: none
// outputs: none
void GameEngine_Init(void)
{
	int i;
	SysTick_Init(2666665);	//It initializes the SysTick for 30 Hz
	for (i=0; i<5; i++)
	{
		playerShip.shoots[i].PosX = SHIPW;	// It makes the position of the shoots to be at Y=0 and
		playerShip.shoots[i].PosY = 0;			// X = the width of the ship
	}
	for (i=0; i<50; i++)
	{
		terrain.backgroundStars[i].PosX = 0;	// It makes the position of 50 possible stars to be 0,0
		terrain.backgroundStars[i].PosY = 0;
		terrain.backgroundStars[i].show = false;
	}
	terrain.minGroundH = MAXGROUND - 10;		// It initializes the upper border of the terrain (ground) to be 10 
																					// pixels above the border down, where the HUD starts
	terrain.ground[0].PosX = 0;							// It makes the position of the first ground pixel to be 0,MAXGROUND, that means it 
																					// starts down with the HUD
	terrain.ground[0].PosY = MAXGROUND;
	for (i=1; i<SCREENW; i++)								// For every other border pixel, we make the position X to be i, so we fill the 
	{																				// screen with border pixels			
		terrain.ground[i].PosX = i;						
		terrain.ground[i].PosY = _GroundNextY(terrain.ground[i-1].PosY, terrain.minGroundH); // The coordinate Y is random and depends
	}																																											 // of the coordinate Y of the last pixel
	terrain.groundCounter = 0;		// We set the ground counter to be 0, so the next ground pixel will "steal" the place of the pixel located at X=0
																// Very important that it is 0, else we get a bug
	playerShip.shCounter = 0;			// We set the quantity of shown shoots to be 0
	playerShip.healthPoints = 3;	// Initial HP of the ship
	playerShip.score = 0;					// Initial score of the ship
	
	animations.playerShipDestruction[0] = (unsigned char*)&PlayerShipDestruction1;  // We point the animation arrays to the individual images on "ImageArrays.h"
	animations.playerShipDestruction[1] = (unsigned char*)&PlayerShipDestruction2;	// So the element 0 of the array points to the first image, the element 1 to
	animations.playerShipDestruction[2] = (unsigned char*)&PlayerShipDestruction3;	// the second image and the element 3 to the third image.
	animations.playerShipDestructionCounter = 0;
	
	for (i=0; i<5; i++)
	{
		enemy[i].posX = SCREENW - ENEMYW - 2; 							// We initialize all enemies to be at the right border of the screen
		enemy[i].posY = 10;
		enemy[i].dead = 0;
		enemy[i].image[0] = (unsigned char*)&enemy1Alive1;  // We point the animation arrays to the individual images on "ImageArrays.h"
		enemy[i].image[1] = (unsigned char*)&enemy1Alive2;  
		enemy[i].image[2] = (unsigned char*)&enemy1Alive3;
		enemy[i].image[3] = (unsigned char*)&enemy1Dying1;
		enemy[i].image[4] = (unsigned char*)&enemy1Dying2;
		enemy[i].actStatus = enemyFSM_NoShow;								// We don't show the enemies at the beginning
	}
}

//**********************_ShowHUD***********************
// Shows the HP and the score of the player at the bottom of the screen
// inputs: none
// outputs: none
void _ShowHUD(void)
{
	unsigned char i;
	for (i = 0; i < SCREENW; i++)			// Draws a line at the bottom to separate the HUD from the playing area
	{
			Nokia5110_SetPixel(i, SCREENH - 7);
	}
	Nokia5110_OutString_4x4pix_toBuffer(0, SCREENH - 5, "HP:");
	Nokia5110_OutUDec_4x4pix_toBuffer(15, SCREENH - 5, playerShip.healthPoints);
	Nokia5110_OutChar_4x4pix_toBuffer(20, SCREENH - 5, '/');
	Nokia5110_OutUDec_4x4pix_toBuffer(25, SCREENH - 5, MAXHP);
	Nokia5110_OutString_4x4pix_toBuffer(35, SCREENH - 5, "score:");
	Nokia5110_OutUDec_4x4pix_toBuffer(65, SCREENH - 5, playerShip.score);
}


//**********************_ShowBackground***********************
// Generates and show stars at the background
// inputs: none
// outputs: none
void _ShowBackground(void)
{
	unsigned char i;
	if ((interruptCounter%5) == Random32()%5) // If the modulo of interruptCounter%5 is equal to a random number between 1 and 4
	{
		terrain.backgroundStars[terrain.starCounter].show = true; 							//We show a star
		terrain.backgroundStars[terrain.starCounter].PosX = SCREENW - 1;				//With x start posision at the end of the screen
		terrain.backgroundStars[terrain.starCounter].PosY = Random32()%(SCREENH-7);	//And y start position a number between 0 and SCREENH - 8
		terrain.starCounter++;
		if (terrain.starCounter >= 50) terrain.starCounter = 0;									//We make place for 50 stars, if a 51 star is created, the star 0 is overwritten
	}
	for (i=0; i<50; i++)																											//We do this for every star
	{
		if ((terrain.backgroundStars[i].show) && ((interruptCounter%2)==0))			//If the star is meant to be shown and the interruptCounter is even 
		{
			Nokia5110_SetPixel(terrain.backgroundStars[i].PosX, terrain.backgroundStars[i].PosY); 		// We print two pixels representing the stars
			Nokia5110_SetPixel(terrain.backgroundStars[i].PosX - 1, terrain.backgroundStars[i].PosY);
			terrain.backgroundStars[i].PosX--;																		// We move the positionX a pixel behind
			if (terrain.backgroundStars[i].PosX < 1)															// If the star reached the end of the display we dont show it anymore
				terrain.backgroundStars[i].show = false;
		}
	}
}

//**********************_ShowTerrain***********************
// Generates and show the terrain on the screen
// inputs: none
// outputs: none
void _ShowTerrain(void)
{
	unsigned char i, j;
	unsigned char PosYLast;				//Coordinate Y of the ground pixel used to generate the coordinate of the next one
	if (interruptCounter%5 == 0)	//It is executed every 5 interrupts, so every 166,67 ms, causing a sensation that it moves
																//slower than the shoots
	{
		for (i = 0; i < SCREENW; i++)	//We move the ground pixels 1 coordinate to the left
		{
			terrain.ground[i].PosX--;
		}
		terrain.ground[terrain.groundCounter].PosX = SCREENW - 1;		//We set the coordinate X of the new pixel to be at the rightmost border
		if (terrain.groundCounter > 0)																//If the pixel we will change now is not the number 0 of the array
		{																															//we use the previous pixel in the array to generate the coordinate Y
			PosYLast = terrain.ground[terrain.groundCounter - 1].PosY;	//of the new ground pixel
		}
		else																													//If the pixel we will change now IS THE NUMBER 0 of the array
		{																															//We use the last pixel of the array to generate the coordinate Y
			PosYLast = terrain.ground[SCREENW - 1].PosY;								//of the new ground pixel
		}
		terrain.ground[terrain.groundCounter].PosY = _GroundNextY(PosYLast, terrain.minGroundH);	//This function generates a random value between the previous
																																														  // and the following Y coordinate
		terrain.groundCounter++;																									//We increase the groundCounter by one, meaning that we will change the next 
																																							//element of the array in the next iteration
		if (terrain.groundCounter >= SCREENW) terrain.groundCounter = 0;  	//If the counter reaches the length of the array, we set it back to 0, to process again the 
																																				//first pixel on the array
	}
	for (i=0; i<SCREENW; i++)																//We draw this in every iteration. For every ground pixel we set all the pixels under it creating a 
																													//mountain on the display
	{
		for (j = terrain.ground[i].PosY; j < MAXGROUND; j++)	
		{
			Nokia5110_SetPixel(terrain.ground[i].PosX, j);
		}
	}
}


//**********************_ControlShip***********************
// This function do the following tasks:
// - Draws the ship and moves it on dependance of the slide pot
// - Generate the shoots of the ship on dependance of the shoot switch
// inputs: none
// outputs: none
void _ControlShip(void)
{
	int i;
	//%%%%%%%%%%%%% MOVEMENT OF THE SHIP %%%%%%%%%%%%%%%%% 
	PixelY = SlidePot_toPixelY(SHIPH);							// Converts the ADC data into readable distance value and then to a position in the Y axis
	if (Nokia5110_AskPixel(8,PixelY-(SHIPH/2)) ||	 	// If the pixel for the frontal point of the ship is already set
			Nokia5110_AskPixel(3,PixelY) ||							// or if pixel for the inferior peak is already set 
			Nokia5110_AskPixel(3,PixelY-SHIPH))					// or if pixel for the superior peak is already set
	{
		// We print the elements of the destruction animation
		Nokia5110_PrintBMP(0, PixelY, animations.playerShipDestruction[animations.playerShipDestructionCounter], 0);
		if (interruptCounter%20==0)
		{
			animations.playerShipDestructionCounter++;	// If interrupt
		}
		if (animations.playerShipDestructionCounter == 3)
		{
			Sound_Explosion();
			playerShip.healthPoints--;
			animations.playerShipDestructionCounter = 0;
			playerShip.dead = true;
		}
		return;
	}
	Nokia5110_PrintBMP(0, PixelY, PlayerShipNew, 0);  		// Draws the ship in the display using the value from the slide pot
	
	//%%%%%%%%%%%%%%%%%% NORMAL SHOOTS %%%%%%%%%%%%%%%%%%%
	if (Switch_shoot)																				// If the switch is pressed we do the following operations on the next shoot on the array
	{
		playerShip.shoots[playerShip.shCounter].show = true;							// We make showingShoot true, this variable is used to show the shoot in the display until it dissapears
		Sound_Shoot();																															// We play the shoot sound by pressing the switch
		playerShip.shoots[playerShip.shCounter].PosY = PixelY - (SHIPH/2);  // We set the position Y of the shoot equals to the center of the ship
		playerShip.shoots[playerShip.shCounter].PosX = SHIPW;								// We set the position X of the shoot equals to the pick of the ship
		playerShip.shCounter++;
		if (playerShip.shCounter >= 5) playerShip.shCounter = 0;							// If ShootCounter is bigger or equal to 5, it is setted again to 0
		Switch_shoot = false;																	// We set back the value of the switch
	}
	for (i=0; i<5; i++)						// For every possible shoot
	{
		if (playerShip.shoots[i].show)		// If the shoot should be shown
		{
			Nokia5110_SetPixel(playerShip.shoots[i].PosX, playerShip.shoots[i].PosY);				// We print three pixels which move until they go out of the screen
			Nokia5110_SetPixel(playerShip.shoots[i].PosX + 1, playerShip.shoots[i].PosY);	
			Nokia5110_SetPixel(playerShip.shoots[i].PosX + 2, playerShip.shoots[i].PosY);
			playerShip.shoots[i].PosX++;																		
			if (playerShip.shoots[i].PosX >= SCREENW)
				playerShip.shoots[i].show = false;													// if the shoot reaches the end of the screen, we turn off the value indicating that a shoot should be shown
		}
	}
}


//**********************_ControlEnemy***********************
// This function do the following tasks:
// - Draws the enemy
// - Moves it randomly  
// - Generates its shoots randomly
// inputs: enemyNumber: Number of the array element of enemy
// outputs: none
void _ControlEnemy(unsigned char enemyNmb)
{
	// %%%%%%%%%%%%%% CONTROL OF DEATH %%%%%%%%%%%%%%%%
	unsigned char i = enemy[enemyNmb].posY;
	unsigned char oldStatus = enemy[enemyNmb].actStatus;
	while ((i > enemy[enemyNmb].posY - ENEMYH) && (enemy[enemyNmb].dead == 0))
	{
		if (Nokia5110_AskPixel(enemy[enemyNmb].posX - 2, i))
		{
			enemy[enemyNmb].dead = 1;
		}
		i--;
	}
	// %%%%%%%%%%%%%% FSM's NEXT STATUS %%%%%%%%%%%%%%%%%%
	if (interruptCounter%enemyFSM[oldStatus].delay == 0)
	{
		enemy[enemyNmb].actStatus = enemyFSM[oldStatus].next[enemy[enemyNmb].dead];
	}
	// %%%%%%%%%%%%%% enemy's next position %%%%%%%%%%%%%%
	if ((enemy[enemyNmb].actStatus < 3) && (interruptCounter%5 == 0))
	{
		if ((enemy[enemyNmb].posX > ENEMYW) && (enemy[enemyNmb].posX < SCREENW - ENEMYW - 2))
		{
			enemy[enemyNmb].posX += -1 + Random()%3;
		}
		if ((enemy[enemyNmb].posY > ENEMYH) && (enemy[enemyNmb].posY < MAXGROUND))
		{
			enemy[enemyNmb].posY += -1 + Random()%3;
		}
	}
	// %%%%%%%%%%%%%% DRAW ENEMY %%%%%%%%%%%%%%%%
	if (enemy[enemyNmb].actStatus != enemyFSM_NoShow)
	{
		Nokia5110_PrintBMP(enemy[enemyNmb].posX, enemy[enemyNmb].posY, enemy[enemyNmb].image[enemy[enemyNmb].actStatus], 0);
	}
	// %%%%%%%%%%%%%% CREATE NEW ENEMY %%%%%%%%%%%
	else
	{
		unsigned char createNewEnemy = Random()%10;
		if (createNewEnemy == 1)
		{
			enemy[enemyNmb].actStatus = enemyFSM_Alive1;
			enemy[enemyNmb].posX = 2*ENEMYW + Random()%(SCREENW - 3*ENEMYW);
			enemy[enemyNmb].posY = ENEMYH + Random()%(MAXGROUND - ENEMYH);
			if (Nokia5110_AskPixel(enemy[enemyNmb].posX, enemy[enemyNmb].posY))
			{
				enemy[enemyNmb].posY -= 20;
			}
			enemy[enemyNmb].dead = 0;
			Sound_Highpitch();
		}
	}
	
}



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
unsigned char _GroundNextY(unsigned char PosYLast, unsigned char minY)
{
		if (PosYLast + 1 > MAXGROUND)
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