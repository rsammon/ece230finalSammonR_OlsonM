#include "msp.h"
#include "multiLCD.h"
#include "sysTickDelays.h"
#include "csHFXT.h"

/**
 * main.c
 * TODO write header file
 * TODO make wiring diagram
 */

#define CLKFRQ  48000000 //MCLK configured with 48MHz
const char msgOne[] = "HATSUNE";
const char msgTwo[] = "MIKU";
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	configHFXT(); //configures HFXT as the source for SMCLK and MCLK at 48MHz
	/* config LCD with the following specs/configurations
	 * -5x8 dot mode
	 * -2 line mode
	 * -4 bit mode
	 * -data connected to port 4H
	 * -RS connected to pin 5.7
	 * -E connected to pin 5.6
	 */
	LCD lcd1;
	lcd1.CONFIG = BIT3;
	lcd1.RSPORT = PB;
	lcd1.RSMASK = 0x200;
	lcd1.EPORT = PB;
	lcd1.EMASK = 0x100;
	setPortLCD(&lcd1, 2);

	configLCD(&lcd1, CLKFRQ);

	initLCD(&lcd1);

	printStringLCD(&lcd1, msgOne, sizeof(msgOne)/sizeof(msgOne[0]));
	newLine(&lcd1);
	printStringLCD(&lcd1, msgTwo, sizeof(msgTwo)/sizeof(msgTwo[0]));

	while(1){
	    delayMilliSec(1000);
	}
}
