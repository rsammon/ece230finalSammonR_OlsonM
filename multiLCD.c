/*
 * multiLCD.c
 *
 *  Created on: Apr 30, 2024
 *      Author: sammonrh
 */

#include "multiLCD.h"
#include "sysTickDelays.h"

#define NONHOME_MASK        0xFC
#define SHIFT16   8
#define UPMASK 0xF0
#define FULLMASK  0xFF
#define DOWNMASK 0x0F
#define UPPERSHIFT 4

#define LONG_INSTR_DELAY    2000
#define SHORT_INSTR_DELAY   50

/*!
 * Delay method based on instruction execution time.
 *   Execution times from Table 6 of HD44780 data sheet, with buffer.
 *
 * \param mode RS mode selection
 * \param instruction Instruction/data to write to LCD
 *
 * \return None
 */
void instructionDelay(uint8_t mode, uint8_t instruction) {
    // if instruction is Return Home or Clear Display, use long delay for
    //  instruction execution; otherwise, use short delay
    if ((mode == DATA_MODE) || (instruction & NONHOME_MASK)) {
        delayMicroSec(SHORT_INSTR_DELAY);
    }
    else {
        delayMicroSec(LONG_INSTR_DELAY);
    }
}

/*!
 * Function that writes instructions to the LCD
 * \param lcdI Pointer to lcd struct that describes the lcd to write to
 * \param mode Either DATA_MODE (0) or COMMAND_MODE (1) depending on the type of instruction to be written
 * \param instruction The instruction to write to the lcd
 *
 * \return none
 */
void writeInstruction(LCD * lcdI, uint8_t mode, uint8_t instruction){
    //TODO make write instruction work for 4 bit mode (rn it only writes the top bits)
    //set data on port
    if (lcdI->CONFIG & BIT1){ //lcd is in 4 bit mode
        //TODO fix this this is just flat out wrong idk what i was thinking
        //i may be stupid but at least im not smart
           if(lcdI->CONFIG & BIT2){
               lcdI->PORT->OUT_H = ( ((lcdI->CONFIG & BIT1) ? DOWNMASK:UPMASK) & lcdI->PORT->OUT_H) | (instruction << ((lcdI->CONFIG & BIT1) ? UPPERSHIFT : 0) );
           } else {
               lcdI->PORT->OUT_L = ( ((lcdI->CONFIG & BIT1) ? DOWNMASK:UPMASK) & lcdI->PORT->OUT_H) | (instruction << ((lcdI->CONFIG & BIT1) ? UPPERSHIFT : 0) );
           }
       }  else { //lcd is in 8 bit mode
           if(lcdI->CONFIG & BIT2){ //lcd is in high port mode
               lcdI->PORT->OUT_H = instruction;
           } else { //lcd is in low port mode
               lcdI->PORT->OUT_L = instruction;
           }
       }
    //set RS if in data mode, reset RS if in control mode
    if(mode == DATA_MODE){
        lcdI->RSPORT->OUT |=lcdI->RSMASK;
    } else {
        lcdI->RSPORT->OUT &=~(lcdI->RSMASK);
    }
    //pulse E on to execute instruction on lcd
    lcdI->EPORT->OUT |= lcdI->EMASK;
    delayMicroSec(1);
    lcdI->EPORT->OUT &= ~(lcdI->EMASK);
    //delay to wait until instruction is finished executing
    instructionDelay(mode, instruction);
}

/*!
 * Function to write command instruction to LCD.
 * \param lcd Pointer to lcd struct describing the lcd to write to
 * \param command Command instruction to write to LCD
 *
 * \return None
 */
void commandInstruction(LCD * lcd,uint8_t command) {
    writeInstruction(lcd, CTRL_MODE, command);
}

/*!
 * Function to write data instruction to LCD. Writes ASCII value to current
 *  cursor location.
 * \param lcd Pointer to lcd struct describing the lcd to write to
 * \param data ASCII value/data to write to LCD
 *
 * \return None
 */
void dataInstruction(LCD * lcd, uint8_t data) {
    writeInstruction(lcd, DATA_MODE, data);
}


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
    if(lcdI->CONFIG & BIT1){ //lcd is in 4 bit mode
        //instructions for 4 bit mode found from Figure 24 from the HD44780U datasheet
        delayMilliSec(50); //wait for more than 40ms after Vcc rises to 2.7V
        // actually 4 bit and 8 bit might be the same code TODO figure this one out for me pls

    } else { //lcd is in 8 bit mode
        //instructions for 8 bit mode found from Figure 23 from the HD44780U datasheet
        delayMilliSec(40); //wait for more than 40ms after Vcc rises to 2.7V
        commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK); //function set
        delayMicroSec(5000); // wait for more than 4.1 ms
        commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK); //function set
        delayMicroSec(150); //wait for more than 100 micros
        commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK); //function set
        //display mode set
            //function set to 8 bit mode; line number and dot format from lcd config
        commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK | ((lcdI->CONFIG & BIT4) >> 2) | (lcdI->CONFIG & BIT3));
        commandInstruction(lcdI, DISPLAY_CTRL_MASK); //display off
        commandInstruction(lcdI, CLEAR_DISPLAY_MASK); //clear display
        commandInstruction(lcdI, ENTRY_MODE_MASK | ID_FLAG_MASK); //set entry mode to increment right

        //turn lcd on
        commandInstruction(lcdI, DISPLAY_CTRL_MASK | D_FLAG_MASK);
    }


}

void printCharLCD(LCD * lcd, char input){
    dataInstruction(lcd, input);
}

void setPortLCD(LCD *lcd, uint8_t portNumber){
    uint8_t portConverted = 0xA + (portNumber-1)/2;
    switch(portConverted){ //doesnt work with port J but tbh i honestly dont know what that port is so like who cares lol
        case 0xA:
            lcd->PORT = PA;
            break;
        case 0xB:
            lcd->PORT = PB;
            break;
        case 0xC:
            lcd->PORT = PC;
            break;
        case 0xD:
            lcd->PORT = PD;
            break;
        case 0xE:
            lcd->PORT = PE;
            break;
    }

    if((portNumber) % 2){ //true if lower port (the odd ports) false if the higher port (the even ports)
        lcd->CONFIG &= BIT2;
    } else{
        lcd->CONFIG |= BIT2;
    }
}
void resetLCD(LCD * lcdI){
    //TODO implement maybe from page 23 of the HD44780 datasheet
}
