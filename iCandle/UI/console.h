/**\file console.h
 * Console functions. Excluded from documentation for simplicity reasons.
 */

#ifndef CONSOLE_H
#define CONSOLE_H
#include "TS_vInterface.h"

/** Output a character to console */
//#define ConsolePutChar(c){while (!TI);TI = 0; SBUF = (c);}
#define ConsolePutChar(c) {TerminalWrite(c);}

//#define DebugMessage(c){ConsoleWrite(c);}
#define DebugMessage(c){}

void ConsoleWrite(char *str);
void ConsolePutHex16 (unsigned int value);
void ConsolePutHex8 (unsigned char value);
void ConsolePutUInt(unsigned long value);
void ConsolePutInt(S16 value);


//#define ConsoleWrite(a);
//#define ConsolePutHex16(a);
//#define ConsolePutHex8(a);
//#define ConsolePutChar(a);
//#define ConsolePutUInt(a);



#endif
