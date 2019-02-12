#ifndef SWITCHES_H
#define SWITCHES_H

#include <stdbool.h>

// Initialization of switches connected to PE0 and PE1
// using edge interrupt 

extern bool Switch_shoot;
extern bool Switch_special;

void SwitchesInit(void);

#endif /* SWITCHES_ */
