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
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	configHFXT(); //configures HFXT as the source for SMCLK and MCLK at 48MHz
	/* config LCD with the following specs/configurations
	 * -5x8 dot mode
	 * -2 line mode
	 * -8 bit mode
	 * -data connected to port 4
	 * -RS connected to pin 5.7
	 * -E connected to pin 5.6
	 */
	LCD lcd1;
	lcd1.CONFIG = BIT3 | BIT0;
	lcd1.RSPORT = PC;
	lcd1.RSMASK = BIT7;
	lcd1.EPORT = PC;
	lcd1.EMASK = BIT6;
	setPortLCD(&lcd1, 4);

	configLCD(&lcd1, CLKFRQ);

	initLCD(&lcd1);

	printCharLCD(&lcd1,'H');
	printCharLCD(&lcd1,'a');
	printCharLCD(&lcd1,'t');
	printCharLCD(&lcd1,'s');
	printCharLCD(&lcd1,'u');
	printCharLCD(&lcd1,'n');
	printCharLCD(&lcd1,'e');
	printCharLCD(&lcd1,' ');
	printCharLCD(&lcd1,'M');
	printCharLCD(&lcd1,'i');
	printCharLCD(&lcd1,'k');
	printCharLCD(&lcd1,'u');

	while(1);
}
