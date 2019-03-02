
#include "GameEngineCycle.h"
#include "Pixel.h"
#include "Enemy.h"
#include "Terrain.h"
#include "PlayerShip.h"
#include "../../tm4c123gh6pm.h"
//#include "../controls/SlidePot.h"
#include "../controls/Switches.h"
//#include "../display/Nokia5110.h"
#include "../display/ImageArrays.h"
//#include "../sounds/Sound.h"
//#include "../main/Random.h"

// ************************************************************************************************
// ******************************** DEFINITION OF GLOBAL QUANTITIES *******************************
// ************************************************************************************************

#define MAXHP 4
#define MAXGROUND 41  //The ground can be drawn maximal in this Y position

// ************************************************************************************************
// ******************************** DECLARATION OF GLOBAL VARIABLES *******************************
// ************************************************************************************************

//******************** GLOBAL VARIABLES *****************************
bool Switch_shoot;		//The switch for shooting was pressed
bool Switch_special;  //The switch for special attack was pressed
extern bool Flag;			//Flag sent to main task when the display should be drawn
unsigned char PixelY;	//This value has the position Y of the ship
unsigned long interruptCounter; // It counts how many sysTick interrupts have been occured

Terrain terrain;				//Object used to represent the terrain
Enemy enemy[5];							//Object used to represent the enemies
PlayerShip playerShip;


// ************************************************************************************************
// ******************************** DECLARATION OF PRIVATE FUNCTIONS ******************************
// ************************************************************************************************

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


// ************************************************************************************************
// *********************************** DEFINITION OF FUNCTIONS ************************************
// ************************************************************************************************

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
	if (playerShip.dead == 2)
	{
		if (playerShip.healthPoints)
		{
				Nokia5110_OutString_4x4pix_toBuffer(10, 15, "You were hurt");
				Nokia5110_OutString_4x4pix_toBuffer(15, 25, "Press shoot");
				Nokia5110_OutString_4x4pix_toBuffer(10, 30, "to try again!");
				if (Switch_shoot)
				{
					Enemy_Clear(&enemy[0]);
					Enemy_Clear(&enemy[1]);
					Enemy_Clear(&enemy[2]);
					playerShip.dead = false;
					playerShip.curStatus = 0;
				}
		}
		else 
		{
			Nokia5110_OutString_4x4pix_toBuffer(10, 20, "GAME OVER :(");
		}
	}
	else 
	{
		Terrain_ShowTerrain(&terrain, interruptCounter, MAXGROUND);
		
		Enemy_NextState(&enemy[0],interruptCounter);
		Enemy_NextPos(&enemy[0],interruptCounter, MAXGROUND);
		Enemy_Draw(&enemy[0],MAXGROUND);		
		Enemy_NextState(&enemy[1],interruptCounter);
		Enemy_NextPos(&enemy[1],interruptCounter, MAXGROUND);
		Enemy_Draw(&enemy[1],MAXGROUND);
		Enemy_NextState(&enemy[2],interruptCounter);
		Enemy_NextPos(&enemy[2],interruptCounter, MAXGROUND);
		Enemy_Draw(&enemy[2],MAXGROUND);
		
		PlayerShip_Shoots(&playerShip);
		
		Enemy_Shoots(&enemy[0]);
		Enemy_Shoots(&enemy[1]);		
		Enemy_Shoots(&enemy[2]);
		
		PlayerShip_ControlShip(&playerShip, interruptCounter);
		PlayerShip_Draw(&playerShip);
		
		Enemy_ControlDeath(&enemy[0]);
		Enemy_ControlDeath(&enemy[1]);
		Enemy_ControlDeath(&enemy[2]);
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
	PlayerShip_InitShip(&playerShip, PlayerShipCenter, PlayerShipUp, PlayerShipDown,
																	 PlayerShipDestruction1, PlayerShipDestruction2, PlayerShipDestruction3);
	
	Terrain_InitTerrain(&terrain, MAXGROUND);
	
	for (i=0; i<5; i++)
	{
		Enemy_InitEnemy(&enemy[i], enemy1Alive1, enemy1Alive2, enemy1Alive3, enemy1Dying1, enemy1Dying2);
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

