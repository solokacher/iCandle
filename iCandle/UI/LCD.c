//Virtualtool LCD display

#include "compiler_defs.h"             // Compiler dependent macros
#include "C8051F020_defs.h"
#include "LCD.h"

/** hexadecimal character set */
const unsigned char code lcd_hexchars[17]="0123456789abcdef";


void LCD_Init (void)
{
   LCD_ControlWrite(0x0C);             // Display ON, Cursor OFF
   LCD_ControlWrite(0x01);             // Clear LCD
   LCD_ControlWrite(0x06);             // Entry mode increment without shift
}


/** Write a Code Constant string to LCD */

void LCDPutString(char *str){
  while (*str)
    {
      LCD_DataWrite(*str++);
    }
}

/** Print a 16-bit hex value on LCD */
void LCDPutHex16 (unsigned int value){
  LCD_DataWrite(lcd_hexchars[value>>12]);
  LCD_DataWrite(lcd_hexchars[(value>>8)&0xf]);
  LCD_DataWrite(lcd_hexchars[(value>>4)&0xf]);
  LCD_DataWrite(lcd_hexchars[(value)&0xf]);
}


/** Print an 8-bit hex value on LCD */
void LCDPutHex8 (unsigned char value){

  LCD_DataWrite(lcd_hexchars[(value>>4)&0xf]);
  LCD_DataWrite(lcd_hexchars[(value)&0xf]);
}

/** Print a 16-bit unsigned integer value on LCD */
void LCDPutUInt(unsigned long value){
  xdata unsigned char valueString[10];
  char c;
  for (c=0; c<10; c++){
    valueString[c]=value % 10;
    value = value / 10;
  }
  c=9;
  while ((valueString[c]==0) && (c!=0))
    c--;
  for (;c!=0;c--)
    LCD_DataWrite('0'+valueString[c]);
    LCD_DataWrite('0'+valueString[0]);
  //LCD_DataWrite(' ');

}

/** Print a 16-bit signed integer value */
void LCDPutInt(S16 value)
{
	if(value < 0)
	{
		LCD_DataWrite('-');
		value = -value;
	}
	LCDPutUInt(value);
}

//void LCDShowMode(S16 value)

