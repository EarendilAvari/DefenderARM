// Sound.c
// Runs on any computer
// Sound assets based off the original Space Invaders and basic
// functions to play them.  Copy these functions and constants
// into your SpaceInvaders.c for ease of sharing your game!
// Jonathan Valvano
// November 19, 2012

#include "../../tm4c123gh6pm.h"
#include "Sound.h"
#include "SoundArrays.h"


unsigned long Index;
const unsigned char *Wave;
unsigned long SoundCounter;

// You can use this timer only if you learn how it works
void Timer2_Init(unsigned long period)
{ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
	// interrupts enabled in the main program after all devices initialized
	// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void Timer2A_Start(void)
{
	TIMER2_CTL_R |= 0x00000001;
}

void Timer2A_Stop(void)
{
	TIMER2_CTL_R &= ~0x00000001;
}

void Timer2A_Handler(void)
{	
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
	if(SoundCounter)
	{
    DAC_Out(Wave[Index]>>4);
    Index = Index + 1;
    SoundCounter = SoundCounter - 1;
  }
	else
	{
		Timer2A_Stop();           // Disables the timer
  }
}

void Sound_Init(void)
{
  DAC_Init();               			 // initialize simple 4-bit DAC
  Timer2_Init(80000000/11025);     // 11.025 kHz
  Index = 0;
  SoundCounter = 0;
}
void Sound_Play(const unsigned char *pt, unsigned long count)
{
  Wave = pt;
  Index = 0;
  SoundCounter = count;
	Timer2A_Start();
}
void Sound_Shoot(void)
{
  Sound_Play(shoot,4080);
}

void Sound_Killed(void)
{
  Sound_Play(invaderkilled,3377);
}


void Sound_Explosion(void)
{
  Sound_Play(explosion,2000);
}


void Sound_Fastinvader1(void)
{
  Sound_Play(fastinvader1,982);
}

void Sound_Fastinvader2(void)
{
  Sound_Play(fastinvader2,1042);
}

void Sound_Fastinvader3(void)
{
  Sound_Play(fastinvader3,1054);
}

void Sound_Fastinvader4(void)
{
  Sound_Play(fastinvader4,1098);
} 

void Sound_Highpitch(void)
{
  Sound_Play(highpitch,1802);
}

