
#include "../../tm4c123gh6pm.h"
//#include "../controls/SlidePot.h"
#include "../controls/Switches.h"
//#include "../display/Nokia5110.h"
#include "../display/ImageArrays.h"
#include "../display/LED.h"
#include "GameEngineCycle.h"
//#include "../sounds/Sound.h"
//#include "../main/Random.h"

// ************************************************************************************************
// ******************************** DECLARATION OF GLOBAL VARIABLES *******************************
// ************************************************************************************************

//******************** GLOBAL VARIABLES *****************************
bool Switch_shoot;		//The switch for shooting was pressed
bool Switch_special;  //The switch for special attack was pressed
extern bool ExecuteMain;			//Flag sent to main task when the display should be drawn
unsigned char PixelY;	//This value has the position Y of the ship
unsigned long interruptCounter; // It counts how many sysTick interrupts have been occured
unsigned short timeToSpecial; 	// Time to unlock special attack 
extern unsigned short difficulty;			// Difficulty of the game, it changes with the score of the player

extern Terrain terrain;				//Object used to represent the terrain (extern because is used in the main thread also)
extern Enemy enemy[5];				//Object used to represent the enemies (extern because is used in the main thread also)
extern PlayerShip playerShip; //Object used to represent the player ship (extern because is used in the main thread also)


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
	LED_SetGreen();		// Green LED is turned on when the SysTick interrupt starts
	Nokia5110_SaveLastBuffer();		//Screen buffer is saved to be used by the Enemy's and PlayerShip's methods
	Nokia5110_ClearBuffer();			//Screen buffer is cleared to be drawn again in this cycle
	if (PlayerShip_isDead(&playerShip))
	{
		if (PlayerShip_hasLives(&playerShip))
		{
				Nokia5110_OutString_4x4pix_toBuffer(10, 15, "You were hurt");
				Nokia5110_OutString_4x4pix_toBuffer(15, 25, "Press shoot");
				Nokia5110_OutString_4x4pix_toBuffer(10, 30, "to try again!");
				if (Switch_shoot)
				{
					Enemy_Reset(&enemy[0]);
					Enemy_Reset(&enemy[1]);
					Enemy_Reset(&enemy[2]);
					PlayerShip_Reset(&playerShip);
					Switch_shoot = false;
					Nokia5110_ClearBuffer();
				}
		}
		else 
		{
			Nokia5110_OutString_4x4pix_toBuffer(10, 20, "GAME OVER :(");
		}
	}
	else 
	{
		Terrain_Create(&terrain, interruptCounter, MAXGROUND);
		Enemy_NextState(&enemy[0],interruptCounter);
		Enemy_NextPos(&enemy[0],interruptCounter, MAXGROUND);
		Enemy_NextState(&enemy[1],interruptCounter);
		Enemy_NextPos(&enemy[1],interruptCounter, MAXGROUND);
		Enemy_NextState(&enemy[2],interruptCounter);
		Enemy_NextPos(&enemy[2],interruptCounter, MAXGROUND);
		PlayerShip_ControlShip(&playerShip, interruptCounter);
		if (difficulty > 2)
		{
			Enemy_NextState(&enemy[3],interruptCounter);
			Enemy_NextPos(&enemy[3], interruptCounter, MAXGROUND);
		}
		if (difficulty > 4)
		{
			Enemy_NextState(&enemy[4], interruptCounter);
			Enemy_NextPos(&enemy[4], interruptCounter, MAXGROUND);
		}
		if (timeToSpecial < 606)
		{
			timeToSpecial++;
			Switch_special = false;
		}
		else
		{
			PlayerShip_specialShoot(&playerShip, interruptCounter, &timeToSpecial, MAXGROUND);
		}
	}
	GameEngine_ShowHUD(interruptCounter, timeToSpecial);
	GameEngine_IncreaseDifficulty(&playerShip, &difficulty);
	Nokia5110_OutUDec_4x4pix_toBuffer(35, 5, difficulty);
	ExecuteMain = true;										// Sets the flag to 1, indicating that the main loop should be executed
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
	SysTick_Init(2666665);	//It initializes the SysTick for 30 Hz
	PlayerShip_InitShip(&playerShip, PlayerShipCenter, PlayerShipUp, PlayerShipDown,
																	 PlayerShipDestruction1, PlayerShipDestruction2, PlayerShipDestruction3);
	PlayerShip_InitSpecialShoot(&playerShip, SpecialShootMiddle1, SpecialShootMiddle2, SpecialShootUp1,
																					 SpecialShootUp2, SpecialShootDown1, SpecialShootDown2);
	
	Terrain_InitTerrain(&terrain, MAXGROUND);
	
	Enemy_InitEnemy(&enemy[0], enemy1Alive1, enemy1Alive2, enemy1Alive3, enemy1Dying1, enemy1Dying2);
	Enemy_InitEnemy(&enemy[1], enemy2Alive1, enemy2Alive2, enemy2Alive3, enemy2Dying1, enemy2Dying2);
	Enemy_InitEnemy(&enemy[2], enemy3Alive1, enemy3Alive2, enemy3Alive3, enemy3Dying1, enemy3Dying2);
	Enemy_InitEnemy(&enemy[3], enemy1Alive1, enemy1Alive2, enemy1Alive3, enemy1Dying1, enemy1Dying2);
	Enemy_InitEnemy(&enemy[4], enemy2Alive1, enemy2Alive2, enemy2Alive3, enemy2Dying1, enemy2Dying2);
	
	timeToSpecial = 606;
	difficulty = 0;
}

//**********************GameEngine_ShowHUD***********************
// Shows the HP and the score of the player at the bottom of the screen
// inputs: timerSpecialAttack: time needed to unlock special attack
//				 intCounter: Indicates how many cycles of the game engine have occurred
// outputs: none
void GameEngine_ShowHUD(unsigned long intCounter, unsigned short timerSpecialAttack)
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
	Nokia5110_OutString_4x4pix_toBuffer(35, SCREENH - 5, "sc:");
	Nokia5110_OutUDec_4x4pix_toBuffer(50, SCREENH - 5, playerShip.score);
	if ((timerSpecialAttack >= 606) && (intCounter%20 < 10))
	{
		Nokia5110_OutString_4x4pix_toBuffer(70, SCREENH - 5, "SP"); 
	}
}

//**********************GameEngine_IncreaseDifficulty***********************
// Shows the HP and the score of the player at the bottom of the screen
// inputs: player: Pointer to the playership object
//				 diff: Pointer to difficulty variable
// outputs: none
void GameEngine_IncreaseDifficulty(PlayerShip* player, unsigned short* diff)
{
	static unsigned short LastScore;
	
	if ((player->score != LastScore) && (player->score%5 == 0))
	{
		*diff = *diff + 1;
	}
	LastScore = player->score;
}
