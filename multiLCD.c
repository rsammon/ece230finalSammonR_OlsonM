/*
 * multiLCD.c
 *
 *  Created on: Apr 30, 2024
 *      Author: sammonrh
 */

#include "multiLCD.h"
#include "sysTickDelays.h"

#define SHIFT16   8
#define UPMASK 0xF0
#define FULLMASK  0xFF
#define DOWNMASK 0x0F
#define UPPERSHIFT 4

void configLCD(LCD* lcdI, uint32_t clkFreq){
    //init EN to low
    /*
     * SEL0 <- 0
     * SEL1 <- 0
     * DIR  <- 1
     * OUT  <- 0
     */
    lcdI->EPORT->SEL0 &= ~(lcdI->EMASK);
    lcdI->EPORT->SEL1 &= ~(lcdI->EMASK);
    lcdI->EPORT->DIR |= lcdI->EMASK;
    lcdI->EPORT->OUT &= ~(lcdI->EMASK);
    //config port pins as GPIO
    /* SEL0 <- 0
     * SEL1 <- 0
     * DIR  <- 1
     * OUT <-  0
     */
    if (lcdI->CONFIG & BIT1){ //lcd is in 4 bit mode
        if(lcdI->CONFIG & BIT2){
            lcdI->PORT->SEL0_H &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            lcdI->PORT->SEL1_H &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            lcdI->PORT->DIR_H |= ((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            //lcdI->PORT->OUT_H &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
        } else {
            lcdI->PORT->SEL0_L &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            lcdI->PORT->SEL1_L &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            lcdI->PORT->DIR_L |= ((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            //lcdI->PORT->OUT_L &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
        }
    }  else { //lcd is in 8 bit mode
        if(lcdI->CONFIG & BIT2){ //lcd is in high port mode
            lcdI->PORT->SEL0_H = 0;
            lcdI->PORT->SEL1_H = 0;
            lcdI->PORT->DIR_H = 0xFF;
            //lcdI->PORT->OUT_H = 0;
        } else { //lcd is in low port mode
            lcdI->PORT->SEL0_L = 0;
            lcdI->PORT->SEL1_L = 0;
            lcdI->PORT->DIR_L = 0xFF;
            //lcdI->PORT->OUT_L = 0;
        }
    }
    //init RS pin
    /*
        * SEL0 <- 0
        * SEL1 <- 0
        * DIR  <- 1
        * OUT  <- 0
    */
    lcdI->RSPORT->SEL0 &= ~(lcdI->RSMASK);
    lcdI->RSPORT->SEL1 &= ~(lcdI->RSMASK);
    lcdI->RSPORT->DIR |= lcdI->RSMASK;
    //lcdI->RSPORT->OUT &= ~RSMASK;

    //delay timer init
    initDelayTimer(clkFreq);
}

void initLCD(LCD* lcdI){
    //8 bit vs 4 bit
    //clear display
    //entry mode set?
    //display mode set

}

