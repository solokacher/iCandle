/**\file lcd.h
 * LCD functions
 */

#ifndef LCD_H
#define LCD_H

#include "TS_vInterface.h"

#define LCDClean(void) LCD_ControlWrite(0x01)
#define LCDChangeLine(void) LCD_ControlWrite (0xC0)


void LCDPutHex16 (unsigned int value);
void LCDPutHex8 (unsigned char value);
void LCDPutUInt(unsigned long value);
void LCDPutInt(S16 value);
void LCDPutString(char *str);


#endif

