//Hardware Abstraction Layer

#include "HAL.h"
#include "../UI/console.h"

/** Read and return an octet from SPI port*/
unsigned char SPIGetChar(){
  SPIWait();
  SPIF = 0;
  SPIPutCharWithoutWaiting(0xff); /* send 0xff */
  SPIWait(); /* wait for the byte to transfer */
  return SPI_RESULT_BYTE; /* Return the received byte */
}

/** Send nClocks x 8 clock transitions with MOSI=1 (0xff) */
void SPI8Clocks(unsigned char nClocks){
  while (nClocks--){
    SPIPutCharWithoutWaiting(0xff);
    SPIWait();
  }
}

/** Busy loop delay function. Creates n times 1 ms hardware delays*/
void Delay(int milliseconds){
  for (;milliseconds;milliseconds--){  /* for n times do: */
    InitiateDelay (1000);
    WaitOutDelay(); /* 1 ms delay */
    }
}

void SetLED(U8 led)
{
	P5 &= 0x0F;
	P5 |= ((led<<4)&0xF0);
}


U8 GetKey()
{
	static U8 KeyFlag = 0;
	static U8 KeyCode = 0;
	if(KeyFlag == 0)
	{
		KeyCode = (~P5)&0x0F;
		if(KeyCode != 0)
		{
			Delay(10);
			if(KeyCode == ((~P5)&0x0F))
			{
				KeyFlag = 1;
				return (KeyCode|KM_HOLD);
			}
			return 0;
		}
	}
	else if(KeyFlag == 1)
	{
		if(((~P5)&0x0F) == 0x00)
		{
			Delay(10);
			if(((~P5)&0x0F) == 0x00)
			{
				KeyFlag = 0;
				return KeyCode;
			}
		}
		return (KeyCode|KM_HOLD);
	}
	return 0;
}

void ADC0_Init (void)
{
	ADC0CN  = 0x8C;     // ADC0 enabled; trigger by Timer 2
	REF0CN  = 0x03;     // Enable on-chip VREF,and VREF output buffer
	AMX0CF  = 0x00;     // AIN inputs are single-ended (default)
	AMX0SL  = 0x02;     // Select AIN0.2 pin as ADC mux input
	ADC0CF  = 0x80;     // ADC conversion clock = 941kHz 10, Gain=1
	EIE2   |= 0x02;     // enable ADC interrupts
	EIP2   |= 0x02;
	ADC0    = 0x00;
	//SETUP TIMER2
	RCAP2  = -0x56CE; //INITIAL NUMBER (2M/12)/10
	T2     = -0x56CE; //RELOAD NUMBER
}


// Initialize the Microcontroller
void InitMCU()
{
  WDTCN = 0xde;		//stop watchdog
  WDTCN = 0xad;

  P0MDOUT |= 0x34;
  P1MDOUT |= 0x00;
  P2MDOUT |= 0xfe;
  P3MDOUT |= 0xff;
  P74OUT = 0x08;

  P1 |= 0xFF;
  P2 |= 0x01;
  P3 |= 0xFF;
  P4 |= 0x00;
  P5 = 0x0F;
  XBR0 = 0x06; //-- Enable UART0 
  XBR1 = 0x00; 
  XBR2 = 0x40;

  ADC0_Init ();
  MP3_XRESET = 0;

  SPI0CFG = 0x07;	//CKPOL=CKPHA=0; 8bit per frame
  SPI0CN = 0x03;	//SPI enable, Master Mode
  SPISetSlowClock();
  EA=0;

  SilabsInit020();
  LCD_Init ();                        // Initialize LCD
  TerminalWrite(0x81);                // Clean console
  ConsoleWrite("WELCOME TO iCandle(R) MUSIC PLAYER\n\n");
  ConsoleWrite("Initializing...\n");
  TR2 =1;
  SDDeselect();
  Mp3DeselectControl();
  Mp3DeselectData();
  SPI8Clocks(1);	//Send a dummy byte to initialize SPI

  InitiateDelay(0);

  ConsoleWrite("MCU intialization complete.\n");

}