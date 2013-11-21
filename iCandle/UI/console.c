#include "../HAL/HAL.h"
#include "console.h"

/**\file console.c
 * Console function implementations.
 */ 

/** hexadecimal character set */
const unsigned char code hexchars[17]="0123456789abcdef";


/** Write a Code Constant string to console */
void ConsoleWrite(char *str){
  while (*str)
    {
      ConsolePutChar (*str++);
    }
}

/** Print a 16-bit hex value on Console */
void ConsolePutHex16 (unsigned int value){
  ConsolePutChar(hexchars[value>>12]);
  ConsolePutChar(hexchars[(value>>8)&0xf]);
  ConsolePutChar(hexchars[(value>>4)&0xf]);
  ConsolePutChar(hexchars[(value)&0xf]);
}


/** Print an 8-bit hex value on Console */
void ConsolePutHex8 (unsigned char value){

  ConsolePutChar(hexchars[(value>>4)&0xf]);
  ConsolePutChar(hexchars[(value)&0xf]);
}

/** Print a 16-bit unsigned integer value on Console */
void ConsolePutUInt(unsigned long value){
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
    ConsolePutChar('0'+valueString[c]);
  ConsolePutChar('0'+valueString[0]);
  //ConsolePutChar(' ');

}

/** Print a 16-bit signed integer value */
void ConsolePutInt(S16 value)
{
	if(value < 0)
	{
		ConsolePutChar('-');
		value = -value;
	}
	ConsolePutUInt(value);
}







