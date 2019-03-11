This is a game based on the classic arcade game Defender. It was developed for the Edx course “Embedded Systems - Shape The World: Multi-Threaded Interfacing” from professor Dr. Jonathan Valvano and Dr. Ramesh Yerraballi.

A demonstration of the game: https://youtu.be/wyMWwUWHzJU

The game is run on a Launchpad TM4C123 of Texas Instruments which uses an ARM Cortex M4F 32-bit microcontroller with 80 MHz clock frequency, 256 KiB Flash memory, 32 KiB RAM and 2 KiB of EEPROM.

The additional hardware consists of: 
-	One Nokia 5110 display used for development: It communicates with the microcontroller through port A (pins 2, 3, 5, 6 and 7) using synchronous serial interface (SSI).
-	One 4 bit binary weighted digital to analog converter (DAC): The microcontroller sends the sound signals with sampling rate of 11 kHz through port B (pins 0, 1, 2 and 3). A timer with interrupt is used to send the outputs in the right time.
-	One potentiometer: Used to move the player. Implemented with an analog to digital converter sampled with frequency 30 Hz. Port E (pin 2) is used as input of the microcontroller to receive the voltage values.
-	Two switches: One used to make normal shoots, other to make special shoots. Implemented with positive edge triggered interrupts. Port E (pin 0 and 1) is used as input for that. 
-	Two LEDs: The green one is ON when a cycle is running, this is used to measure the execution time of the game. The yellow one is ON when a sound is being reproduced. Port B (pin 4 and 5) are used as output for that.

The software is written almost completely in C with some routines in Assembler and consist of:
-	Drivers for the display, the DAC, the potentiometer, the switches and the LEDs. The initial file of the display driver was given by the professors as part of the course material and it was extended by me.
-	A random number generator.
-	Main thread where all drivers and modules are initialized, and some actions are executed.
-	Game engine executed every 33,3 ms (30 Hz). 
-	The game engine is written object-oriented including classes for Enemy, Player ship and Terrain. There is one player ship and one terrain executed in the game and five enemies. 

Some highlights of the software are:
-	The generation of terrain is random. The height of the next column can be 1 pixel lower, equal or 1 pixel higher than the last one. The maximum height of the terrain increases with the score of the player. That means, the playable surface becomes shorter.
-	The movement of the enemies is random in both axes. The position on the X axis can change 1 pixel to the left, 1 pixel to the right or stay as it is. The position on the Y axis can change 1 pixel up, 1 pixel down or stay as it is. If the enemy is at one border, it cannot move further in direction to that border.
-	The initial position of the enemies is random in both axes. First a random position X and Y is generated. After that is verified if there is something there already, so the new enemy does not appear above another enemy or above the terrain. Then the enemy is created.
-	Whether the enemies shoot or not is random. In every program cycle for every enemy a “dice” of 70 numbers is thrown, and if the number got is 1, the enemy shoots. The frequency, the enemies shoot with, increases with the score of the player, that is done by reducing the possible numbers of the “dice”.
-	Whether the enemies appear or not after they are killed is random. In every program cycle for every enemy not being shown currently a “dice” of 70 numbers is thrown, and if the number got is 1, the new enemy is “reset” and shown as a new enemy. The probability an enemy appears after killed increases with the score of the player, that is done by reducing the possible numbers of the “dice”.
-	The special shoot can be executed only every 20 seconds at the beginning. This time increases by 5 seconds with every 10 points the player gets. There is a blinking indication at the right bottom corner of the display when the special shoot is ready to be used.

The program is executed in four threads: the main thread, the SysTick interrupt thread, the sound’s timer thread and the switches interrupt thread. The priorities are from highest to lowest in the following order: sound’s timer thread, switches thread, SysTick interrupt thread and main thread.

In the SysTick thread are the following actions executed:
-	Set height of the terrain.
-	Create the terrain.
-	Determine new state of the enemies.
-	Determine new position of the enemies.
-	Determine new state of the player ship.
-	Determine new position of the player ship.
-	Execute special shoot of the player ship.
-	Show HUD.
-	Increase difficulty based on score of the player.

In the main thread are the following actions executed:
-	Initialize drivers and other software modules.
-	Draw terrain created on SysTick thread.
-	Draw enemies.
-	Draw player ship.
-	Draw normal shoots of the player ship.
-	Draw shoots of the enemies.
-	Determine if an enemy die or not.
-	Increase score of the player based on killed enemies. 
-	Send display buffer to the display.

