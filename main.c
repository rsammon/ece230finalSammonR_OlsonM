#include "msp.h"
#include "multiLCD.h"
#include "sysTickDelays.h"
#include "csHFXT.h"
#include "csLFXT.h"
#include <string.h>
#include <stdio.h>

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
LCD scorelcd;

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
	 * -4 bit mode
	 * -data connected to lower port 4
	 * -RS connected to pin 6.0
	 * -RW connected to pin 6.1
	 * -E connected to pin 4.4
	 */

	scorelcd.CONFIG = BIT3 | BIT5 | BIT0;
	scorelcd.RSPORT = PC;
	scorelcd.RSMASK = BIT0 << 8;
	scorelcd.EPORT = PB;
	scorelcd.EMASK = BIT4 << 8;
	scorelcd.RWPORT = PC;
	scorelcd.RWMASK = BIT1 << 8;
	setPortLCD(&scorelcd, 4);
	configLCD(&scorelcd, CLKFRQ);
	initLCD(&scorelcd);

    /* config LCD with the following specs/configurations
     * -5x8 dot mode
     * -2 line mode
     * -4 bit mode
     * -data connected to lower port 4
     * -RS connected to pin 5.2
     * -RW connected to pin 5.1
     * -E connected to pin 5.0
     */

	notelcd.CONFIG = BIT3 | BIT5 | BIT0;
	notelcd.RSPORT = PC;
	notelcd.RSMASK = BIT2;
	notelcd.EPORT = PC;
	notelcd.EMASK = BIT0;
	notelcd.RWPORT = PC;
	notelcd.RWMASK = BIT1;
	setPortLCD(&notelcd, 2);
	configLCD(&notelcd, CLKFRQ);
	initLCD(&notelcd);

	setupNoteScrolling();
	setupButtons();


    __enable_irq();

   SCROLL_TIMER->CTL |= 0x10; //Start note scrolling timer
   score = 0;
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
        else score--;
    }

    if(RBUT_PORT->IFG & RBUT_MASK){
        RBUT_PORT->IFG &= ~RBUT_MASK; //reset interrupt flag
        if(rightLane[15] == '>') score++;
        else score--;
    }

    if(score ==65535) score =0;
    //update score
    returnCursor(&scorelcd);
    char scoreString[16];
    sprintf(scoreString, "Score: %8d",score);
    printStringLCD(&scorelcd, scoreString, sizeof(scoreString)/sizeof(scoreString[0]));

}
