//Hardware Abstraction Layer

#ifndef _HAL_H_
#define _HAL_H_


#include "compiler_defs.h"
#include "C8051F020_defs.h"
#include "pins_defs.h"

/* MP3 Player Chip Connections */

/** Chip Reset */
#define MP3_XRESET P2_4

/** Put the MP3 player chip in reset */
#define Mp3PutInReset(){MP3_XRESET=0;}

/** Release the MP3 player chip from reset */
#define Mp3ReleaseFromReset(){MP3_XRESET=1;}

/** Control Chip Select (for accessing SPI Control/Status registers) */
#define MP3_XCS P2_3

/** Pull the VS10xx Control Chip Select line Low */
#define Mp3SelectControl(){MP3_XCS=0;}

/** Pull the VS10xx Control Chip Select line High */
#define Mp3DeselectControl(){MP3_XCS=1;}

/** Data Chip Select / BSYNC */
#define MP3_XDCS P2_5

/** Pull the VS10xx Data Chip Select line Low */
#define Mp3SelectData(){MP3_XDCS=0;}
//void Mp3SelectData();

/** Pull the VS10xx Data Chip Select line High */
#define Mp3DeselectData(){MP3_XDCS=1;}
//void Mp3DeselectData();

/** Data Request: Player request for more data */
#define MP3_DREQ P2_0


/* 
 * LED Connections
 */

//#define LED
void SetLED(U8 led);


/*
 * Button connections
 */
/*
#warning Nicky chaged bottom
#define BTN_PAUSE		!(P5&0x01)
#define BTN_PREVIOUS	!(P5&0x02)
#define BTN_NEXT		!(P5&0x04)
#define BTN_MODE		!(P5&0x08)
*/
#define KEY_PAUSE		1
#define KEY_NEXT		2
#define KEY_PREVIOUS	4
#define KEY_MODE		8

#define KM_CLK		0
#define KM_DBLCLK	0x40
#define KM_HOLD		0x80

U8 GetKey();	//Get the key code



/*
 * LCD Connections
 *
 *
 */
#warning TO BE IMPLEMENTED IF NECESSARY



/*
 * Various connections 
 */

/** RS-232 Buffer Enable */ 
//#define RSEN P2_1



/*
 * SD connections 
 */


/** YES = 1 */
#define YES 1

/** NO = 0 */
#define NO 0

/** MMC Off (MMC Power Off) */
#define SD_OFF P2_7

/** MMC Chip Select (Active Low)*/
#define SD_CS P2_6

/** MMC is selected when MMC_XCS is low */
#define SD_SELECTED 0

/** MMC is not selected when MMC_XCS is high */
#define SD_NOT_SELECTED 1


/** Pull the VS10xx Data Chip Select line Low */
#define SDSelect(){SD_CS = SD_SELECTED;}

/** Pull the VS10xx Data Chip Select line High */
#define SDDeselect(){SD_CS = SD_NOT_SELECTED;}




/* Utility macros */



/* Delay system */


/** Clear PCA core counter overflow flag */
#define ClearPCAOverflow() {CF=0;}

/** Start Programmable Counter Array core counter. */
#define PCARun() {CR=1;}

/** Stop Programmable Counter Array core counter. */
#define PCAHalt() {CR=0;}

/** Stop PCA core counter and clear overflow flag.  */
#define PCAHaltAndClearOV() {PCA0CN &= 0x3F;}


/** Set PCA core counter value */
#define SetPCAValue(v) {PCA0H = ((unsigned)(v)) >> 8;  PCA0L = (v) & 0xff;}

/** Calculate and set Time-Until-Overflow to PCA in microseconds */
#define SetPCADelayValue(microseconds) {SetPCAValue(65535-(unsigned int)((microseconds)/0.5425347));}
//0.5425347 is for SYSCLK=22.1184MHz, divided by 12

/** Initiate Delay of n microseconds */
#define InitiateDelay(microseconds) {PCAHaltAndClearOV(); SetPCADelayValue(microseconds); PCARun();}

/** Busy loop until the end of the delay time. 
 * In the delay system, the PCA core counter is used for hardware timing. 
 * First call InitiateDelay to put the counter running. Next do whatever
 * work you can do while the delay is lasting. Lastly call WaitOutDelay to
 * busy loop until the hardware says that the delay time has passed.
 * This allows interrupt routines etc. to run during the delay without
 * wasting unnecessary cycles at the delay loop. 
 * \warning Do not call from interrupt routines!
 */ 
#define WaitOutDelay(){while (!CF);}


/***************************
 *        SPI macros       *
 ***************************/
//Clear SPIF before Transmition
#define SPIStartTrans() {SPIF=0;}

/** switch to fast SPI Clock */
#define SPISetFastClock(){SPI0CKR = 2;}

//switch to slow SPI Clock
#define SPISetSlowClock() {SPI0CKR = 96;}


/** Wait for SPI ready to send a new character (previous sent) */
#define SPIWait(){while(!SPIF);}

/** Wait for SPI ready, then initiate character sending */
#define SPIPutChar(c){SPIWait();SPIF=0;SPI0DAT=(c);}

/** Initiate SPI character sending */
#define SPIPutCharWithoutWaiting(c) {SPIF=0;SPI0DAT=(c);}

/** SPI data return register */
#define SPI_RESULT_BYTE SPI0DAT

/** Send 8 ones to SPI bus and receive the result byte. */
unsigned char SPIGetChar();

/** Send nClocks x 8 clock transitions with MOSI=1 (0xff) */
void SPI8Clocks(unsigned char nClocks);


/* Utility functions */

/** Busy loop delay function. Creates n times 1 ms hardware delays */
void Delay (int milliseconds);

#define Delay_us(t) {InitiateDelay(t);WaitOutDelay();}

/** Startup initialization of the microcontroller board */
void InitMCU();



/** "Public" has no code meaning but it is meant for the programmer
 * to remember that this function is called from outside the module */
#define Public



#endif
