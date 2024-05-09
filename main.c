#include "msp.h"
#include "multiLCD.h"
#include "sysTickDelays.h"
#include "csHFXT.h"
#include "csLFXT.h"
#include <string.h>

/**
 * main.c
 * TODO write header file
 * TODO make wiring diagram
 */



#define CLKFRQ  48000000 //MCLK configured with 48MHz
#define ACLKFRQ 32000   //ACLK configured with 32kHz

#define LCD_SIZE    16
 char rightLane[LCD_SIZE];
char leftLane[LCD_SIZE];
int mapPos;
const char rightLaneMap[] = ">                 >       > >>>>>   >>>  |";
const char leftLaneMap[] = ">                   >       > >>>>>   >>>|";

LCD notelcd;

volatile uint16_t score;

#define BPM                 400
#define NOTE_SPEED          BPM/4
#define SCROLL_TIMER        TIMER_A1
#define PRESCALER           2

#define TIMER_NOTE_SPEED    60*ACLKFRQ/(PRESCALER*NOTE_SPEED)

/*!
 * sets up the timer and other stuff for the note scrolling
 */
void setupNoteScrolling(){
    /*configure timer
     * -SMCLK source
     * - prescaler defined above
     */
    SCROLL_TIMER->CCTL[0] =  0x10; // 0000 0000 0001 0000
    SCROLL_TIMER->CCR[0] = TIMER_NOTE_SPEED;
    SCROLL_TIMER->CTL = 0x0146; //0000 0001 0100 0110

    NVIC->ISER[0] |= 0x1 << TA1_0_IRQn; //enable interrupts globally

    mapPos = 0; //set to first part of note map

    //fill lanes with spaces
    int i;
    for(i = 0; i < LCD_SIZE; i++){
        rightLane[i] = ' ';
        leftLane[i] = ' ';
    }
}

#define LBUT_PORT      P3
#define LBUT_MASK      BIT0
#define RBUT_PORT      P3
#define RBUT_MASK      BIT2
/*!
 * sets up the input buttons with interrupts
 */
void setupButtons(){
    //set buttons as inputs with a pullup resistor
    LBUT_PORT->SEL0 &= ~LBUT_MASK;
    LBUT_PORT->SEL1 &= ~LBUT_MASK;
    LBUT_PORT->DIR &= ~LBUT_MASK;
    LBUT_PORT->REN &= ~LBUT_MASK;
    LBUT_PORT->OUT &= ~LBUT_MASK;

    RBUT_PORT->SEL0 &= ~RBUT_MASK;
    RBUT_PORT->SEL1 &= ~RBUT_MASK;
    RBUT_PORT->DIR &= ~RBUT_MASK;
    RBUT_PORT->REN &= ~RBUT_MASK;
    RBUT_PORT->OUT &= ~RBUT_MASK;

    //enable interrupts
    LBUT_PORT->IES |= LBUT_MASK;
    LBUT_PORT->IE  |= LBUT_MASK;
    RBUT_PORT->IES |= RBUT_MASK;
    RBUT_PORT->IE  |= RBUT_MASK;
    NVIC->ISER[1] |= 0x1 << (PORT3_IRQn-32);
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	configHFXT(); //configures HFXT as the source for SMCLK and MCLK at 48MHz
	configLFXT(); //configures LFXT as the source for ACLK
	/* config LCD with the following specs/configurations
	 * -5x8 dot mode
	 * -2 line mode
	 * -8 bit mode
	 * -data connected to port 2
	 * -RS connected to pin 4.1
	 * -RW connected to pin 4.2
	 * -E connected to pin 4.0
	 */

	notelcd.CONFIG = BIT3 | BIT5;
	notelcd.RSPORT = PB;
	notelcd.RSMASK = 0x200;
	notelcd.EPORT = PB;
	notelcd.EMASK = 0x100;
	notelcd.RWPORT = PB;
	notelcd.RWMASK = 0x400;
	setPortLCD(&notelcd, 2);
	configLCD(&notelcd, CLKFRQ);
	initLCD(&notelcd);

	setupNoteScrolling();
	setupButtons();
	score = 0;

    __enable_irq();

   SCROLL_TIMER->CTL |= 0x10; //Start note scrolling timer
   while(1);
}

void TA1_0_IRQHandler(void) {
    SCROLL_TIMER->CCTL[0] &= 0xFE; //reset flag

    //shift notes down
    int i;
    for(i = LCD_SIZE - 1; i > 0; i--){
        rightLane[i] = rightLane[i-1];
        leftLane[i] = leftLane[i-1];
    }
    rightLane[0] = rightLaneMap[mapPos];
    leftLane[0] = leftLaneMap[mapPos];
    mapPos++;
    if(mapPos > strlen(leftLaneMap) - 1) mapPos = 0; //loops if map goes out of bounds TODO temp fix, should be changed to end the song

    /*
     * PRINT CHARACTERS
     */
    returnCursor(&notelcd);
    printStringLCD(&notelcd, rightLane, LCD_SIZE+1);
    newLine(&notelcd);
    printStringLCD(&notelcd,leftLane, LCD_SIZE+1);
}

void PORT3_IRQHandler(void) {
    //check which interrupt was triggered
    if(LBUT_PORT->IFG & LBUT_MASK){
        LBUT_PORT->IFG &= ~LBUT_MASK; //reset interrupt flag
        if(leftLane[15] == '>') score++;
    }

    if(RBUT_PORT->IFG & RBUT_MASK){
        RBUT_PORT->IFG &= ~RBUT_MASK; //reset interrupt flag
        if(rightLane[15] == '>') score++;
    }
}
