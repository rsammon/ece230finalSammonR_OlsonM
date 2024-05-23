#include "msp.h"
#include "multiLCD.h"
#include "sysTickDelays.h"
#include "csHFXT.h"
#include "csLFXT.h"
#include <string.h>
#include <stdio.h>

#define NOTECNT 277
// Note A4 - 440 Hz, B4 - 493.88 Hz, C5 - 523.26 Hz
#define REST 0
#define NOTEA3  54545
#define NOTED4  40864
#define NOTEE4  36404
#define NOTEFSHARP4  32433
#define NOTEG4  30612
#define NOTEGSHARP4  28895
#define NOTEA4  27273
#define NOTEASHARP4  25742
#define NOTEB4  24297
#define NOTEC5  22934
#define NOTECSHARP5  21646
#define NOTED5  20431
#define NOTEDSHARP5  19285
#define NOTEE5  18203
#define NOTEFSHARP5  16216
#define NOTEG5  15306
#define NOTEA5  13636
#define NOTEASHARP5 12871
#define NOTEB5  12149
const uint16_t noteFreqs[277] = {NOTEFSHARP5, NOTECSHARP5, NOTEFSHARP5, NOTECSHARP5,
                                NOTEFSHARP5, NOTECSHARP5, NOTEFSHARP5, NOTECSHARP5,
                                NOTEFSHARP5, NOTECSHARP5, NOTEFSHARP5, NOTECSHARP5,
                                NOTEB4, NOTEA4, NOTECSHARP5,
                                NOTEA4, NOTEB4, NOTEE5, NOTEDSHARP5,
                                NOTEE5, NOTEFSHARP5, NOTEDSHARP5, NOTEB4,
                                NOTEFSHARP5, NOTEB4, NOTEFSHARP5, NOTEB4,
                                NOTEFSHARP5, NOTEB4, NOTEFSHARP5, NOTEASHARP4,
                                NOTEFSHARP5, NOTEASHARP4, NOTEG5,
                                NOTEFSHARP5, NOTED5, NOTEFSHARP5, NOTED5,
                                NOTEE5, NOTEFSHARP5, NOTEE5, REST,
                                NOTED5, NOTECSHARP5,


                                NOTEFSHARP5, NOTECSHARP5, NOTEFSHARP5, NOTECSHARP5,
                                NOTEFSHARP5, NOTECSHARP5, NOTEFSHARP5, NOTECSHARP5,
                                NOTEFSHARP5, NOTECSHARP5, NOTEFSHARP5, NOTECSHARP5,
                                NOTEB4, NOTEA4, NOTECSHARP5,
                                NOTEA4, NOTEB4, NOTEE5, NOTEDSHARP5,
                                NOTEE5, NOTEFSHARP5, NOTEDSHARP5, NOTEB4,
                                NOTEFSHARP5, NOTEB4, NOTEFSHARP5, NOTEB4,
                                NOTEFSHARP5, NOTEB4, NOTEFSHARP5, NOTEASHARP4,
                                NOTEFSHARP5, NOTEASHARP4, NOTEG5,
                                NOTEFSHARP5, NOTED5, NOTEFSHARP5, NOTED5,
                                NOTEE5, NOTEFSHARP5, NOTECSHARP5, REST,
                                NOTEFSHARP5, NOTECSHARP5,

                                NOTED5, NOTEA4,
                                NOTEB4, NOTECSHARP5, NOTED5,
                                NOTECSHARP5, NOTED5,
                                NOTEA4, NOTEB4,
                                NOTEA4, NOTEG4, NOTEFSHARP4,
                                NOTEFSHARP5, NOTEE5,
                                NOTED5, NOTEA4,
                                NOTEB4, NOTECSHARP5, NOTED5,
                                NOTECSHARP5, NOTED5,
                                NOTEFSHARP5, NOTEG5,
                                NOTEFSHARP5, NOTEE5, NOTED5,
                                NOTEE5, NOTECSHARP5,
                                NOTED5, NOTEA4,
                                NOTEB4, NOTECSHARP5, NOTED5,
                                NOTECSHARP5, NOTED5,
                                NOTEA4, NOTEB4, //MEASURE 33
                                NOTEA4, NOTEG4, NOTEFSHARP4,
                                NOTEFSHARP5, NOTEE5,
                                NOTED5, NOTEA4,
                                NOTEB4, NOTECSHARP5, NOTED5,
                                NOTECSHARP5, NOTED5,
                                NOTEFSHARP5, NOTED5, NOTEFSHARP5, NOTED5,
                                NOTEE5, NOTEFSHARP5, NOTEE5,
                                NOTED5, NOTECSHARP5,
                                NOTEA4, NOTEA5,
                                NOTEE5,
                                NOTED5, NOTEA5,
                                NOTEA4,
                                NOTEA4, NOTED5,
                                NOTEA5, NOTEB5,
                                NOTEA5, NOTEE5,
                                NOTED5,
                                NOTEA4, NOTEA5,
                                NOTEE5, NOTED5,
                                NOTEE5, NOTEFSHARP5, NOTED5, NOTEE5,
                                NOTED5, NOTEA5, NOTEA4, NOTED5,
                                NOTEE5, NOTEFSHARP5, NOTEE5, NOTEA5,
                                NOTEFSHARP5, NOTEG5,
                                NOTEFSHARP5, NOTEE5,
                                NOTED5, NOTEE5,
                                NOTEFSHARP5, NOTEG5,
                                NOTEB5, NOTEA5,

                                NOTEG4, NOTEG5, NOTEE5, NOTED5,
                                NOTEC5, NOTEB4, NOTEC5,
                                NOTED5, NOTEB5,
                                NOTEASHARP5, NOTEA5, NOTEG5,
                                NOTEA4, NOTEG5, NOTEE5, NOTED5,
                                NOTEC5, NOTED5, NOTEE5,
                                NOTEB4, NOTED5, NOTED5,
                                NOTEC5, NOTEB4, NOTEC5,

                                NOTEG4, NOTEG5, NOTEE5, NOTED5,
                                NOTEC5, NOTEB4, NOTEC5,
                                NOTED5, NOTEB5,
                                NOTEASHARP5, NOTEA5, NOTEG5,
                                NOTEA4, NOTEG5, NOTEE5, NOTED5,
                                NOTEC5, NOTED5, NOTEE5,
                                NOTEB4, NOTED5, NOTED5,
                                NOTEC5, NOTEB4, NOTEC5,

                                NOTEA4, NOTED4, NOTEE4, NOTED4, NOTEGSHARP4,
                                NOTED4, NOTEE4, NOTED4, NOTEG4, NOTED4,
                                NOTEE4, NOTEG4, NOTEFSHARP4, NOTEE4, NOTED4, NOTEA3,

                                NOTEA4, NOTED4, NOTEE4, NOTED4, NOTEGSHARP4,
                                NOTED4, NOTEE4, NOTED4, NOTEG4, NOTED4,
                                NOTEE4, NOTEG4, NOTEFSHARP4, NOTEE4, NOTED4, REST,
                                NOTEA4, NOTED4, NOTEE4, NOTED4, REST,
                                NOTED4, NOTEE4, NOTEFSHARP4, NOTEE4, NOTED4};


#define DOTTED_HALF_NOTE          58982
#define DOTTED_QUARTER_NOTE       29491
#define QUARTER_NOTE              19661
#define EIGHTH_NOTE               9830
#define SIXTEENTH_NOTE            4915

const uint16_t notePeriods[277] = {EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,

                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,

                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,
                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,
                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,
                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,
                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,
                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE, //MEASURE 33
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,
                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  QUARTER_NOTE, QUARTER_NOTE,

                                  DOTTED_QUARTER_NOTE, DOTTED_QUARTER_NOTE,
                                  DOTTED_HALF_NOTE,
                                  DOTTED_QUARTER_NOTE, DOTTED_QUARTER_NOTE,
                                  DOTTED_HALF_NOTE,
                                  DOTTED_QUARTER_NOTE, DOTTED_QUARTER_NOTE,
                                  DOTTED_HALF_NOTE, EIGHTH_NOTE,
                                  DOTTED_QUARTER_NOTE, DOTTED_QUARTER_NOTE,
                                  DOTTED_HALF_NOTE,
                                  DOTTED_QUARTER_NOTE, DOTTED_QUARTER_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  DOTTED_QUARTER_NOTE, DOTTED_QUARTER_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE, EIGHTH_NOTE,
                                  DOTTED_QUARTER_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, DOTTED_QUARTER_NOTE,
                                  QUARTER_NOTE, EIGHTH_NOTE,
                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, DOTTED_HALF_NOTE,

                                  DOTTED_QUARTER_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,
                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, DOTTED_HALF_NOTE,
                                  DOTTED_QUARTER_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,
                                  DOTTED_QUARTER_NOTE, DOTTED_QUARTER_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,

                                  DOTTED_QUARTER_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,
                                  DOTTED_QUARTER_NOTE, EIGHTH_NOTE,
                                  EIGHTH_NOTE, EIGHTH_NOTE, DOTTED_HALF_NOTE,
                                  DOTTED_QUARTER_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,
                                  DOTTED_QUARTER_NOTE, DOTTED_QUARTER_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,

                                  QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,
                                  QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,

                                  QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,
                                  QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE,
                                  QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE, EIGHTH_NOTE, QUARTER_NOTE,
                                  EIGHTH_NOTE, QUARTER_NOTE, SIXTEENTH_NOTE, SIXTEENTH_NOTE, DOTTED_HALF_NOTE};
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
uint32_t noteIndex = 0;
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
	volatile uint32_t weakDelay = 0;
	    int noteIndex = 0;

	P2->DIR |= BIT4;            // set P2.4 as output
	P2->SEL0 |= BIT4;           // P2.4 set to TA0.1
	P2->SEL1 &= ~BIT4;

	TIMER_A0->CCR[0] = NOTEA4 - 1;
	TIMER_A0->CCR[1] = (NOTEA4 / 2) - 1;
	TIMER_A0->CCTL[1] |= 0x0060;
	TIMER_A0->CTL = 0x0294;

	TIMER_A2->CCR[2] = notePeriods[0];
	TIMER_A2->CCTL[2] = 0x0010;
	TIMER_A2->CTL = 0x0126;
	NVIC->ISER[0] |= ~0x7FF;

    __enable_irq();

   SCROLL_TIMER->CTL |= 0x10; //Start note scrolling timer
   score = 0;
   while(1){
       for(weakDelay = 1000000; weakDelay > 0; weakDelay--) {

               }
               noteIndex = (noteIndex + 1)%NOTECNT;
               TIMER_A2->CTL |= BIT5;
               TIMER_A0->CCR[0] = notePeriods[noteIndex] - 1;
               TIMER_A0->CCR[1] = (notePeriods[noteIndex] / 2) - 1;
   }
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

void TA2_N_IRQHandler(void)
   {
       int delay = 0;
       /* Check if interrupt triggered by timer overflow */
       if(TIMER_A2->CTL)
       {

           // Clear timer overflow flag
           // TODO clear flag in TA1CTL
           TIMER_A2->CTL &= ~BIT0;

       }
       /* Check if interrupt triggered by CCR2 */
       if(TIMER_A2->CCTL[2] & TIMER_A_CCTLN_CCIFG)
       {
           // Clear CCR2 compare interrupt flag
           TIMER_A2->CCTL[2] &= ~BIT0;

           //Increment note arrays
           noteIndex = (noteIndex + 1)%NOTECNT;
           //Rest for 5ms
           TIMER_A0->CCR[0] = 0;
           TIMER_A0->CCR[1] = 0;
           for( delay = 0; delay < 15000; delay++);
           //Update note length
           TIMER_A2->CCR[2] += notePeriods[noteIndex];
           //Update note frequency
           TIMER_A0->CCR[0] = noteFreqs[noteIndex] - 1;
           TIMER_A0->CCR[1] = (noteFreqs[noteIndex] / 2) - 1;
       }
   }
