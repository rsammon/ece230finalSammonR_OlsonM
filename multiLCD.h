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
 * - port information
 * - bit mode (8 bit or
 */
typedef struct {
    //16 bit port type (e.g. PA, PB)
    DIO_PORT_Interruptable_Type * PORT;
    //16 bit port type (e.g. PA, PB)
    DIO_PORT_Interruptable_Type * RSPORT;
    //offset for the 16 bit port
    char RSMASK;
    //16 bit port type (e.g. PA, PB)
    DIO_PORT_Interruptable_Type * EPORT;
    //offset for the 16 bit port
    char EMASK;
    /* config defines states of the
     * bit 7-3 - reserved
     * bit 2 - lcd is connected to the higher port (1) or the lower port (0)
     * bit 1 - lcd is connected to the upper (1) or lower (0) part of a port (only matters for 4 bit mode)
     * bit 0 - lcd is in 4 bit mode (0) or 8 bit mode (1)
     */
    char CONFIG;
} LCD;
/**
 * TODO add docs
 */
extern void configLCD(LCD* lcdI, uint32_t clkFreq);

/**
 * TODO add docs
 */
extern void initLCD(LCD* lcdI);
#endif /* MULTILCD_H_ */
