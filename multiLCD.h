/*
 * multiLCD.h
 *
 *  Created on: Apr 30, 2024
 *      Author: sammonrh
 */

#ifndef MULTILCD_H_
#define MULTILCD_H_
#include "msp.h"

/*define struct type for a LCD that contains
 * - port/pin information for both data and RS, E, and RW
 * - bit mode (8 bit or 4 bit)
 * - 2/1 line mode
 * - dots mode (5x11 or 5x8)
 * - busy flag or timing mode
 */
typedef struct lcd_structure{
    //16 bit port type (e.g. PA, PB)
    DIO_PORT_Interruptable_Type * PORT;
    //16 bit port type (e.g. PA, PB)
    DIO_PORT_Interruptable_Type * RSPORT;
    //offset for the 16 bit port
    uint16_t RSMASK;
    //16 bit port type (e.g. PA, PB)
    DIO_PORT_Interruptable_Type * EPORT;
    //offset for the 16 bit port
    uint16_t EMASK;
    //16 bit port type (e.g. PA, PB)
    DIO_PORT_Interruptable_Type * RWPORT;
    //offset for the 16 bit port
    uint16_t RWMASK;
    /* config defines states of the
     * bit 7-6- reserved
     * bit 5 - lcd is in busy flag mode (1) or timing mode (0) (busy flag mode is faster/more accurate but requires one extra pin on the MPU,
          timing mode is incredibly inconsistent for 4 bit mode)
     * bit 4 - lcd is 5x11 dots mode (1) or in 5x8 dots mode (0)
     * bit 3 - lcd is in 2 line mode (1) or 1 line mode (0)
     * bit 2 - lcd is connected to the higher port (1) or the lower port (0)
     * bit 1 - lcd is connected to the upper (1) or lower (0) part of a port (only matters for 4 bit mode)
     * bit 0 - lcd is in 4 bit mode (1) or 8 bit mode (0)
     */
    char CONFIG;
} LCD;

/* Instruction masks */
#define CLEAR_DISPLAY_MASK  0x01
#define RETURN_HOME_MASK    0x02
#define ENTRY_MODE_MASK     0x04
#define DISPLAY_CTRL_MASK   0x08
#define CURSOR_SHIFT_MASK   0x10
#define FUNCTION_SET_MASK   0x20
#define SET_CGRAM_MASK      0x40
#define SET_CURSOR_MASK     0x80

/* Field masks for instructions:
 * DL   = 1: 8 bits, DL = 0: 4 bits
 * N    = 1: 2 lines, N = 0: 1 line
 * S/C  = 1: Display shift
 * S/C  = 0: Cursor move
 * F    = 1: 5 x 10 dots, F = 0: 5 x 8 dots
 * R/L  = 1: Shift to the right
 * R/L  = 0: Shift to the left
 * D    = 1: Display On, D = 0: Display Off
 * C    = 1: Cursor On, D = 0: Cursor Off
 * I/D  = 1: Increment
 * I/D  = 0: Decrement
 * B    = 1: Cursor blink On, D = 0: Cursor blink Off
 * S    = 1: Accompanies display shift
 * BF   = 1: Internally operating
 * BF   = 0: Instructions acceptable
 */
#define DL_FLAG_MASK        0x10
#define N_FLAG_MASK         0x08
#define SC_FLAG_MASK        0x08
#define F_FLAG_MASK         0x04
#define RL_FLAG_MASK        0x04
#define D_FLAG_MASK         0x04
#define C_FLAG_MASK         0x02
#define ID_FLAG_MASK        0x02
#define B_FLAG_MASK         0x01
#define S_FLAG_MASK         0x08

//used for setting the modes for instructions
#define CTRL_MODE           0
#define DATA_MODE           1

/*!
 * sets up the pins for the lcd, configures the clock used for delays
 * \param lcdI - the lcd to set up the pins for. requires initialized values for the RS, E, Port, and (if in BF check mode) RW
 * \param clkFreq - the frequency in Hz to set the clock to
 * \return none
 */
extern void configLCD(LCD* lcdI, uint32_t clkFreq);

/*!
 * initializes the lcd, following Figure 23/24 (depending on the bit interface mode)
 * \param lcdI - the lcd to initialize. requires initialized values for the RS, E, Port, and (if in BF check mode) RW
 * \return none
 */
extern void initLCD(LCD* lcdI);

/*!
 * sets the 16 bit port and bit of the config for a specified 8 bit port number.
 * \param lcd - the lcd to set the port in
 * \param portNumber - the number of the 8 bit port number that the lcd is connected to
 * \return none
 */
extern void setPortLCD(LCD *lcd, uint8_t portNumber);

/*!
 * print a character to an lcd at the current cursor position
 * \param lcd - the lcd to print the character on
 * \param input - the character to print
 * \return none
 */
extern void printCharLCD(LCD * lcd, char input);

/*!
 * prints a string to an lcd at the current cursor position
 * \param lcd - the lcd to print the string on
 * \param input - the string to print
 * \param msgLength - the length of the string
 * \return none
 */
extern void printStringLCD(LCD * lcd, const char* input, int msgLength);

/*!
 * clears the lcd and resets the current cursor position
 * \param lcd - the lcd to clear
 * \returns none
 */
extern void clearDisplay(LCD * lcd);

/*!
 * returns the cursor to the beginning of line 1
 * \param lcd - the lcd to return the cursor to
 * \return none
 */
extern void returnCursor(LCD * lcd);

/*!
 * places the cursor at the beginning of line 2 (only works for 2 line lcds)
 * \param lcd - the lcd to create the new line at
 * \return none
 */
extern void newLine(LCD * lcd);

//UNUSED
//extern void shiftRightLCD(LCD * lcd, uint8_t moveCursor);

#endif /* MULTILCD_H_ */
