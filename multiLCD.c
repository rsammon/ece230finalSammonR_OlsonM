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
#define ENABLELENGTH 1

#define LONG_INSTR_DELAY    2000
#define SHORT_INSTR_DELAY   50

/*!
 * sets the data pins to output mode
 * \param LCD* lcd - the lcd to set the data pins as output
 * \return none
 */
void setPinsOut(LCD * lcdI){
    //config port pins as output
    //DIR <- 1
    if (lcdI->CONFIG & BIT0){ //lcd is in 4 bit mode
        if(lcdI->CONFIG & BIT2){
            lcdI->PORT->DIR_H |= ((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
        } else {
            lcdI->PORT->DIR_L |= ((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
        }
    }  else { //lcd is in 8 bit mode
        if(lcdI->CONFIG & BIT2){ //lcd is in high port mode
            lcdI->PORT->DIR_H = 0xFF;
        } else { //lcd is in low port mode
            lcdI->PORT->DIR_L = 0xFF;
        }
    }
}

/*!
 * sets the data pins to input mode
 * \param LCD* lcd - the lcd to set the data pins as input
 * \return none
 */
void setPinsIn(LCD * lcdI){
    //config data pins as input
    /* DIR <-0
     */
    if (lcdI->CONFIG & BIT0){ //lcd is in 4 bit mode
        if(lcdI->CONFIG & BIT2){
            lcdI->PORT->DIR_H &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
        } else {
            lcdI->PORT->DIR_L &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
        }
    }  else { //lcd is in 8 bit mode
        if(lcdI->CONFIG & BIT2){ //lcd is in high port mode
            lcdI->PORT->DIR_H = 0;
        } else { //lcd is in low port mode
            lcdI->PORT->DIR_L = 0;
        }
    }
}
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
 * reads the busy flag from the lcd
 * \param lcd - the lcd struct to read from
 * \return a char containing the busy flag BF in bit7 and the Address Counter AC (BF,AC6-0)
 */
char readBusyFlag(LCD * lcd){
    char dataRead = 0x00;
    setPinsIn(lcd);
    lcd->RSPORT->OUT &= ~(lcd->RSMASK);
    lcd->RWPORT->OUT |= lcd->RWMASK;
    if(lcd->CONFIG & BIT0){ //led is in 4 bit mode
        //pulse E once for the higher nibble of the byte
        lcd->EPORT->OUT |= lcd->EMASK;
        delayMicroSec(ENABLELENGTH);
        //read upper nibble from input
        if(lcd->CONFIG & BIT2){ //in high port
            if(lcd->CONFIG & BIT1){ //in upper of port
                dataRead = (lcd->PORT->IN_H) & UPMASK;
            } else { //in lower part of port
                dataRead = ((lcd->PORT->IN_H) & DOWNMASK) <<4;
            }
        } else { //in low port
            if(lcd->CONFIG & BIT1){ //in upper of port
                dataRead = (lcd->PORT->IN_L) & UPMASK;
            } else { //in lower part of port
                dataRead = ((lcd->PORT->IN_L) & DOWNMASK) <<4;
            }
        }
        lcd->EPORT->OUT &= ~(lcd->EMASK);


        //pulse E once for the lower nibble of the byte
        lcd->EPORT->OUT |= (lcd->EMASK);
        delayMicroSec(ENABLELENGTH);
        //read lower nibble from input
                if(lcd->CONFIG & BIT2){ //in high port
                    if(lcd->CONFIG & BIT1){ //in upper of port
                        dataRead += ((lcd->PORT->IN_H) & UPMASK) >>4;
                    } else { //in lower part of port
                        dataRead += ((lcd->PORT->IN_H) & DOWNMASK);
                    }
                } else { //in low port
                    if(lcd->CONFIG & BIT1){ //in upper of port
                        dataRead += ((lcd->PORT->IN_L) & UPMASK) >>4;
                    } else { //in lower part of port
                        dataRead += ((lcd->PORT->IN_L) & DOWNMASK);
                    }
                }
        lcd->EPORT->OUT &= ~(lcd->EMASK);

    } else {
        //pulse E once for the byte
        lcd->EPORT->OUT |= lcd->EMASK;
        delayMicroSec(ENABLELENGTH);
        if(lcd->CONFIG & BIT2){ //in high port
            dataRead = lcd->PORT->IN_H;
        } else { //in low port
            dataRead = lcd->PORT->IN_L;
        }
        lcd->EPORT->OUT &= ~(lcd->EMASK);


    }

    return dataRead;
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
    setPinsOut(lcdI);
    //set RS if in data mode, reset RS if in control mode
    if(mode == DATA_MODE){
        lcdI->RSPORT->OUT |=lcdI->RSMASK;
    } else {
        lcdI->RSPORT->OUT &=~(lcdI->RSMASK);
    }
    lcdI->RWPORT->OUT &= ~(lcdI->RWMASK);

    //set data on port
    if (lcdI->CONFIG & BIT0){ //lcd is in 4 bit mode

        //write the higher nibble of the instruction
           if(lcdI->CONFIG & BIT2){ //in high port
               if(lcdI->CONFIG & BIT1){ //in upper of port
               lcdI->PORT->OUT_H = (DOWNMASK & lcdI->PORT->OUT_H) | (instruction & UPMASK);
               } else { //in lower part of port
                   lcdI->PORT->OUT_H = (UPMASK & lcdI->PORT->OUT_H) | ((instruction & UPMASK) >> UPPERSHIFT);
               }
           } else { //in low port
               if(lcdI->CONFIG & BIT1){ //in upper of port
                   lcdI->PORT->OUT_L = (DOWNMASK & lcdI->PORT->OUT_L) | (instruction & UPMASK);
               } else { //in lower part of port
                   lcdI->PORT->OUT_L = (UPMASK & lcdI->PORT->OUT_L) | ((instruction & UPMASK) >> UPPERSHIFT);
               }
           }

           //pulse E on to execute first nibble
           lcdI->EPORT->OUT |= lcdI->EMASK;
           delayMicroSec(ENABLELENGTH);
           lcdI->EPORT->OUT &= ~(lcdI->EMASK);


           //write the lower nibble of the instruction
           if(lcdI->CONFIG & BIT2){ //in high port
               if(lcdI->CONFIG & BIT1){ //in upper of port
                  lcdI->PORT->OUT_H = (DOWNMASK & lcdI->PORT->OUT_H) | ((instruction & DOWNMASK) <<UPPERSHIFT);
               } else { //in lower part of port
                      lcdI->PORT->OUT_H = (UPMASK & lcdI->PORT->OUT_H) | (instruction & DOWNMASK);
               }
           } else { //in low port
               if(lcdI->CONFIG & BIT1){ //in upper of port
                   lcdI->PORT->OUT_L = (DOWNMASK & lcdI->PORT->OUT_L) | ((instruction & DOWNMASK) <<UPPERSHIFT);
               } else { //in lower part of port
                   lcdI->PORT->OUT_L = (UPMASK & lcdI->PORT->OUT_L) | (instruction & DOWNMASK);
               }
           }


       }  else { //lcd is in 8 bit mode
           if(lcdI->CONFIG & BIT2){ //lcd is in high port mode
               lcdI->PORT->OUT_H = instruction;
           } else { //lcd is in low port mode
               lcdI->PORT->OUT_L = instruction;
           }
       }

    //pulse E on to execute instruction on lcd
    lcdI->EPORT->OUT |= lcdI->EMASK;
    delayMicroSec(ENABLELENGTH);
    lcdI->EPORT->OUT &= ~(lcdI->EMASK);
    //delay to wait until instruction is finished executing
    if(lcdI->CONFIG & BIT5){
        while(readBusyFlag(lcdI) & BIT7);
    } else {
    instructionDelay(mode, instruction);
    }
}

/*!
 * UNUSED: writes 4 bits to data top 4 pins
 */
void writeInstruction4bit(LCD * lcdI, uint8_t mode, uint8_t instruction){
    setPinsOut(lcdI);
    //write nibble to port
    if(lcdI->CONFIG & BIT2){
        lcdI->PORT->OUT_H = ( ((lcdI->CONFIG & BIT1) ? DOWNMASK:UPMASK) & lcdI->PORT->OUT_H) | (instruction << ((lcdI->CONFIG & BIT1) ? UPPERSHIFT : 0) );
    } else {
        lcdI->PORT->OUT_L = ( ((lcdI->CONFIG & BIT1) ? DOWNMASK:UPMASK) & lcdI->PORT->OUT_H) | (instruction << ((lcdI->CONFIG & BIT1) ? UPPERSHIFT : 0) );
    }
    //set RS if in data mode, reset RS if in control mode
    if(mode == DATA_MODE){
        lcdI->RSPORT->OUT |=lcdI->RSMASK;
    } else {
        lcdI->RSPORT->OUT &=~(lcdI->RSMASK);
    }
    //pulse E on to execute instruction on lcd
    lcdI->EPORT->OUT |= lcdI->EMASK;
    delayMicroSec(ENABLELENGTH);
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

/*!
 * Function to write command instruction to LCD.
 * \param lcd Pointer to lcd struct describing the lcd to write to
 * \param command Command instruction to write to LCD
 *
 * \return None
 */
void commandInstruction4bit(LCD * lcd,uint8_t command) {
    writeInstruction4bit(lcd, CTRL_MODE, command);
}

/*!
 * Function to write data instruction to LCD. Writes ASCII value to current
 *  cursor location.
 * \param lcd Pointer to lcd struct describing the lcd to write to
 * \param data ASCII value/data to write to LCD
 *
 * \return None
 */
void dataInstruction4bit(LCD * lcd, uint8_t data) {
    writeInstruction4bit(lcd, DATA_MODE, data);
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



    //init select pins for lcd data pins
    /* SEL0 <- 0 set to gpio mode
     * SEL1 <- 0
     * REN  <- 0 disable pullup/down resistor
     */
    if (lcdI->CONFIG & BIT0){ //lcd is in 4 bit mode
        if(lcdI->CONFIG & BIT2){
            lcdI->PORT->SEL0_H &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            lcdI->PORT->SEL1_H &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            lcdI->PORT->REN_H &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);

        } else {
            lcdI->PORT->SEL0_L &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            lcdI->PORT->SEL1_L &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
            lcdI->PORT->REN_L &= ~((lcdI->CONFIG & BIT1) ? UPMASK:DOWNMASK);
        }
    }  else { //lcd is in 8 bit mode
        if(lcdI->CONFIG & BIT2){ //lcd is in high port mode
            lcdI->PORT->SEL0_H = 0;
            lcdI->PORT->SEL1_H = 0;
            lcdI->PORT->REN_H = 0;
        } else { //lcd is in low port mode
            lcdI->PORT->SEL0_L = 0;
            lcdI->PORT->SEL1_L = 0;
            lcdI->PORT->REN_L = 0;
        }
    }

    setPinsOut(lcdI);
    //init RS pin
    /*
        * SEL0 <- 0
        * SEL1 <- 0
        * DIR  <- 1
    */
    lcdI->RSPORT->SEL0 &= ~(lcdI->RSMASK);
    lcdI->RSPORT->SEL1 &= ~(lcdI->RSMASK);
    lcdI->RSPORT->DIR |= lcdI->RSMASK;

    if(lcdI->CONFIG & BIT5){
    //init RW pin
        /*
            * SEL0 <- 0
            * SEL1 <- 0
            * DIR  <- 1
        */
        lcdI->RWPORT->SEL0 &= ~(lcdI->RWMASK);
        lcdI->RWPORT->SEL1 &= ~(lcdI->RWMASK);
        lcdI->RWPORT->DIR |= lcdI->RWMASK;
    }

    //delay timer init
    initDelayTimer(clkFreq);
}

void initLCD(LCD* lcdI){
    char BFCheck = lcdI->CONFIG & BIT5; //save BF config
    lcdI->CONFIG &= ~BIT5; //reset BF since cannot be checked until later
    if(lcdI->CONFIG & BIT0){ //lcd is in 4 bit mode
        //instructions for 4 bit mode found from Figure 24 from the HD44780U datasheet
        delayMilliSec(50); //wait for more than 40ms after Vcc rises to 2.7V
        //commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK); //function set
        commandInstruction4bit(lcdI, (FUNCTION_SET_MASK | DL_FLAG_MASK) >>4);
        delayMilliSec(5); // wait for more than 4.1 ms
        //commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK); //function set
        commandInstruction4bit(lcdI, (FUNCTION_SET_MASK | DL_FLAG_MASK) >>4); //function set
        delayMicroSec(150); //wait for more than 100 micros
        //commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK); //function set
        commandInstruction4bit(lcdI, (FUNCTION_SET_MASK | DL_FLAG_MASK) >> 4); //function set

        lcdI->CONFIG |= BFCheck;
        //function set to 4 bit mode; line number and dot format from lcd config
        commandInstruction(lcdI, FUNCTION_SET_MASK);
        commandInstruction(lcdI, FUNCTION_SET_MASK | ((lcdI->CONFIG & BIT4) >> 2) | (lcdI->CONFIG & BIT3));
        commandInstruction(lcdI, DISPLAY_CTRL_MASK); //display off
        commandInstruction(lcdI, CLEAR_DISPLAY_MASK); //clear display
        commandInstruction(lcdI, ENTRY_MODE_MASK | ID_FLAG_MASK); //set entry mode to increment right
        commandInstruction(lcdI, DISPLAY_CTRL_MASK | D_FLAG_MASK); //turn lcd on

    } else { //lcd is in 8 bit mode
        //instructions for 8 bit mode found from Figure 23 from the HD44780U datasheet
        delayMilliSec(50); //wait for more than 40ms after Vcc rises to 2.7V
        commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK); //function set
        delayMicroSec(5000); // wait for more than 4.1 ms
        commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK); //function set
        delayMicroSec(150); //wait for more than 100 micros
        commandInstruction(lcdI, FUNCTION_SET_MASK | DL_FLAG_MASK); //function set

        lcdI->CONFIG |= BFCheck;
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
        lcd->CONFIG &= ~BIT2;
    } else{
        lcd->CONFIG |= BIT2;
    }
}
void resetLCD(LCD * lcdI){
    //TODO implement maybe from page 23 of the HD44780 datasheet
}

void printStringLCD(LCD * lcd, const char * input, int msgLength){
    int i;
    for(i = 0; i < msgLength-1; i++){
        printCharLCD(lcd, input[i]);
    }
}

void clearDisplay(LCD * lcd) {
    // clear the LCD display and return cursor to home position
    commandInstruction(lcd, CLEAR_DISPLAY_MASK);
}

void returnCursor(LCD * lcd){
    commandInstruction(lcd, BIT1);
}

void newLine(LCD * lcd){
    commandInstruction(lcd, BIT7 + 0x40);
}

void shiftLCD(LCD * lcd, uint8_t moveRL, uint8_t moveCursor){
    commandInstruction(lcd, (DL_FLAG_MASK | (moveRL<<2) | (moveCursor <<3) ) );
}

void shiftRightLCD(LCD * lcd, uint8_t moveCursor){
    shiftLCD(lcd, 1, moveCursor);
}
