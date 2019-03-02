
#ifndef LED_H
#define LED_H

// Implementation of two LEDs on PB4 and PB5

// **************LED_Init*********************
// Initialize 2 LEDs to measure times on the game
// Input: none
// Output: none
void LED_Init(void);

// **************LED_SetGreen*********************
// Turns on green LED
// Input: none
// Output: none
void LED_SetGreen(void);

// **************LED_ResetGreen*********************
// Turns off green LED
// Input: none
// Output: none
void LED_ResetGreen(void);

// **************LED_ToggleGreen*********************
// Toggles green LED
// Input: none
// Output: none
void LED_ToggleGreen(void);

// **************LED_SetYellow*********************
// Turns on yellow LED
// Input: none
// Output: none
void LED_SetYellow(void);

// **************LED_ResetYellow*********************
// Turns off yellow LED
// Input: none
// Output: none
void LED_ResetYellow(void);

#endif LED_H

