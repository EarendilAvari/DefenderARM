// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the edX Lab 15
// In order for other students to play your game
// 1) You must leave the hardware configuration as defined
// 2) You must not add/remove any files from the project
// 3) You must add your code only this this C file
// I.e., if you wish to use code from sprite.c or sound.c, move that code in this file
// 4) It must compile with the 32k limit of the free Keil

// April 10, 2014
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2013

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2013

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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
// ******* Required Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE2/AIN1
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Blue Nokia 5110
// ---------------
// Signal        (Nokia 5110) LaunchPad pin
// Reset         (RST, pin 1) connected to PA7
// SSI0Fss       (CE,  pin 2) connected to PA3
// Data/Command  (DC,  pin 3) connected to PA6
// SSI0Tx        (Din, pin 4) connected to PA5
// SSI0Clk       (Clk, pin 5) connected to PA2
// 3.3V          (Vcc, pin 6) power
// back light    (BL,  pin 7) not connected, consists of 4 white LEDs which draw ~80mA total
// Ground        (Gnd, pin 8) ground

// Red SparkFun Nokia 5110 (LCD-10168)
// -----------------------------------
// Signal        (Nokia 5110) LaunchPad pin
// 3.3V          (VCC, pin 1) power
// Ground        (GND, pin 2) ground
// SSI0Fss       (SCE, pin 3) connected to PA3
// Reset         (RST, pin 4) connected to PA7
// Data/Command  (D/C, pin 5) connected to PA6
// SSI0Tx        (DN,  pin 6) connected to PA5
// SSI0Clk       (SCLK, pin 7) connected to PA2
// back light    (LED, pin 8) not connected, consists of 4 white LEDs which draw ~80mA total

#include "../../tm4c123gh6pm.h"

#include "Random.h"
#include "TExaS.h"
#include "StartImage.h"

#include "../display/Nokia5110.h"
#include "../display/LED.h"
#include "../controls/Switches.h"
#include "../controls/SlidePot.h"
#include "../sounds/Sound.h"
#include "../gameEngine/GameEngineCycle.h"
#include "../gameEngine/Enemy.h"
#include "../gameEngine/PlayerShip.h"
#include "../gameEngine/Terrain.h"

bool ExecuteMain;
unsigned short difficulty;			// Difficulty of the game, it changes with the score of the player
unsigned short GroundMaxHeight;		// Max height of the ground
Enemy enemy[5];								//Object used to represent the enemies
PlayerShip playerShip;
Terrain terrain;							//Object used to represent the terrain (extern because is used in the main thread also)

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Timer2_Init(unsigned long period);
void Delay100ms(unsigned long count); // time delay in 0.1 seconds


int main(void)
{
	unsigned long StartCounter = 0;
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Nokia5110_Init();
	SwitchesInit();
	Sound_Init();
	EnableInterrupts();
	Nokia5110_Clear();
	SlidePot_Init();
	LED_Init();
	Nokia5110_PrintBMP(0, 40, StartImage, 0);
	Nokia5110_OutString_4x4pix_toBuffer(15, 35, "PRESS SHOOT");
	Nokia5110_DisplayBuffer();
	LED_SetGreen();
	LED_SetYellow();
	while (!Switch_shoot) 
	{
		StartCounter++;
	}
	Switch_shoot = false;
	LED_ResetGreen();
	LED_ResetYellow();
	Random_Init(StartCounter);
	GameEngine_Init();
	Nokia5110_ClearBuffer();		//Screen buffer is cleared
	Nokia5110_SaveLastBuffer();	//Last screen buffer is cleared
  while(1)
	{
		if (ExecuteMain)					//If the flag in the SysTick interrupt was set, this code is executed
		{
			if (!PlayerShip_isDead(&playerShip))
			{
				Terrain_Draw(&terrain, MAXGROUND);
				Enemy_Draw(&enemy[0],GroundMaxHeight, difficulty);
				Enemy_Draw(&enemy[1],GroundMaxHeight, difficulty);
				Enemy_Draw(&enemy[2],GroundMaxHeight, difficulty);
				
				if (difficulty > 2)
				{
					Enemy_Draw(&enemy[3], GroundMaxHeight, difficulty);
				}
				if (difficulty > 4)
				{
					Enemy_Draw(&enemy[4], GroundMaxHeight, difficulty);
				}
				PlayerShip_Draw(&playerShip);
				
				PlayerShip_Shoots(&playerShip);
		
				Enemy_Shoots(&enemy[0], difficulty);
				Enemy_Shoots(&enemy[1], difficulty);		
				Enemy_Shoots(&enemy[2], difficulty);
				if (difficulty > 2)
				{
					Enemy_Shoots(&enemy[3], difficulty);
				}
				if (difficulty > 4)
				{
					Enemy_Shoots(&enemy[4], difficulty);
				}
				
				PlayerShip_IncreaseScore(&playerShip, Enemy_ControlDeath(&enemy[0]), Enemy_ControlDeath(&enemy[1]), 
																 Enemy_ControlDeath(&enemy[2]), Enemy_ControlDeath(&enemy[3]), Enemy_ControlDeath(&enemy[4]));
			}
			Nokia5110_DisplayBuffer();
			ExecuteMain = false;
		LED_ResetGreen();		//Green LED is turned OFF when main is ready
		}
  }
}

