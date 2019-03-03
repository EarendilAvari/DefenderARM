#ifndef PLAYERSHIP_H
#define PLAYERSHIP_H

// ************************************************************************************************
// ******************************* INTERACTION WITH ANOTHER MODULES *******************************
// ************************************************************************************************
#include "pixel.h"
#include "../Display/Nokia5110.h"
#include "../Sounds/Sound.h"
#include "../Display/Nokia5110.h"
#include "../Controls/Switches.h"
#include "../Controls/SlidePot.h"

// ************************************************************************************************
// *********************************** DECLARATION OF "CLASS" *************************************
// ************************************************************************************************


typedef struct SpecialShootVariables
{
	unsigned char posX;
	unsigned char posY;
	bool show;
	unsigned char curStatus;
	unsigned char *image[2];
}SpecialShoot;

// *************** Structure used for the ship ***************************
typedef struct PlayerShipVariables
{
	unsigned char posX; 				//Position X of the ship
	unsigned char posY; 				//Position Y of the ship
	PixelType shoots[5];				//Array of shoots of the ship
	SpecialShoot specialShootMiddle;
	SpecialShoot specialShootUp;
	SpecialShoot specialShootDown;
	unsigned char shCounter; 		//Counter of shoots
	unsigned char healthPoints; //Health points of the ship
	unsigned short score;				//Score of the ship
	unsigned char dead;					// (0: Alive, 1: Dying, 2: Dead)
	unsigned char curStatus;		// Current status of the ship
	unsigned char *image[6];
}PlayerShip;




// ************************************************************************************************
// ******************************** DECLARATION OF PUBLIC FUNCTIONS *******************************
// ************************************************************************************************

//**********************PlayerShip_ControlShip***********************
// This function performs the following tasks:
// - Determines the next position of the ship
// - Determines whether the player ship is killed in this cycle or not
// - Changes the status of the finite state machine used for the ship
// based on the input from the slide pot and if it is dying or not.
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 intCounter: Indicates how many cycles of the game engine have occurred
// outputs: none
void PlayerShip_ControlShip(PlayerShip *this, unsigned char intCounter);

//**********************PlayerShip_Draw***********************
// This function whether draws the player ship on the display buffer
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
void PlayerShip_Draw(PlayerShip *this);

//**********************PlayerShip_Shoots***********************
// This function generates the shoots of the player ship pointed by this.
// The shoots are shown until they reach the right border of the display.
// The ship can generate until 5 shoots at the same time
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
void PlayerShip_Shoots(PlayerShip *this);


//**********************PlayerShip_InitShip***********************
// This function do the following tasks:
// - Draws the ship and moves it on dependance of the slide pot
// - Generate the shoots of the ship on dependance of the shoot switch
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 img0: image of the ship centered
//				 img1: image of the ship going up
//				 img2: image of the ship going down
// 				 img3: image 1 of destruction animation
//				 img4: image 2 of destruction animation
// 				 img5: image 3 of destruction animation
// outputs: none
void PlayerShip_InitShip(PlayerShip* this, const unsigned char *img0, const unsigned char *img1, const unsigned char *img2,
												 const unsigned char *img3, const unsigned char *img4, const unsigned char *img5);

//**********************PlayerShip_IncreaseScore***********************
// This function increases the score of the player based on the killed status
// of five enemies.
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 enm1killed: Enemy 1 was killed
//				 enm2killed: Enemy 2 was killed
//				 enm3killed: Enemy 3 was killed
//				 enm4killed: Enemy 4 was killed
//				 enm5killed: Enemy 5 was killed
// outputs: none
void PlayerShip_IncreaseScore(PlayerShip* this, bool enm1killed, bool enm2killed, 
															bool enm3killed, bool enm4killed, bool enm5killed);


//**********************PlayerShip_isDead***********************
// This function return 1 if the player was killed, else it returns 0
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
bool PlayerShip_isDead(PlayerShip* this);

//**********************PlayerShip_hasLives***********************
// This function return 1 if the player has lives and therefore can still play, else it returns 0
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
bool PlayerShip_hasLives(PlayerShip* this);

//**********************PlayerShip_Reset***********************
// This function revives the player ship 
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
// outputs: none
void PlayerShip_Reset(PlayerShip* this);

//**********************PlayerShip_InitSpecialShoot***********************
// This function initializes the special shoots
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 imgMid1:  image of the special shoot in the middle 1
//				 imgMid2:  image of the special shoot in the middle 2
//				 imgUp1: 	 image of the special shoot in the top 1
// 				 imgUp2: 	 image of the special shoot in the top 2
//				 imgDown1: image of the special shoot in the bottom 1
// 				 imgDown2: image of the special shoot in the bottom 2
// outputs: none
void PlayerShip_InitSpecialShoot(PlayerShip* this, const unsigned char* imgMid1, const unsigned char* imgMid2,
																									 const unsigned char* imgUp1, const unsigned char* imgUp2,
																									 const unsigned char* imgDown1, const unsigned char* imgDown2);
																									 
																									 
//**********************PlayerShip_specialShoots***********************
// This function generates a special shoot by pressing the special button
// inputs: this: Corresponds to the structure including the parameters of the class, in this case the ship
//				 intCounter: Indicates how many cycles of the game engine have occurred
// outputs: none
void PlayerShip_specialShoot(PlayerShip *this, unsigned long intCounter);

#endif
