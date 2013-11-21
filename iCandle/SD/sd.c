// SD card


#include "../HAL/HAL.h"
#include "sd.h"

#define SPI_receive_m(p)\
{\	
	SPIWait();\
	SPIPutCharWithoutWaiting(0xff);\
	SPIWait();\
	*(p) = SPI_RESULT_BYTE;\
}

/*-------------------------------------------------------------------------------------------------*/
unsigned char SD_init(void)
{
	unsigned char response = 0xff;
	unsigned char retry = 0;
	unsigned char i = 0;
		
	SPISetSlowClock(); /*set SPI clk <= 400kHz*/
	Delay(10);

	SD_CS_DEASSERT;
	SPI8Clocks(10); /*at least 74 clk*/

	do
	{ 	
		response = SD_sendCommand(CMD0, 0); /*send 'reset & go idle' command, the response should be 0x01 if successfull*/
		retry++;
  		if(retry > 0xfe)
			return 1; /*time out*/
   	}while(response != 0x01);

	retry = 0;
	do
	{
		response = SD_sendCommand(CMD1, 0); /*activate card's initialization process, the response should be 0x00 if successfull*/
		Delay(1);
    	response = SD_sendCommand(CMD1, 0); /*same command sent again for compatibility with some cards*/
		retry++;
    	if(retry > 0xfe) 
	  		return 1; /*time out*/
	}while(response);

	SD_sendCommand(CMD16, 512); /*set block size to 512*/

	SPISetFastClock(); /*set SPI clk fast*/
	Delay(10);	//wait for SPI stablizing

	return 0; /*normal return*/
}

/*-------------------------------------------------------------------------------------------------*/
unsigned char SD_sendCommand(unsigned char cmd, unsigned long arg)
{
	unsigned char response = 0xff;
	unsigned char retry = 0;

	SD_CS_ASSERT;

	SPIPutChar(cmd); /*send command, first two bits always '01'*/
	SPIPutChar(arg>>24);
	SPIPutChar(arg>>16);
	SPIPutChar(arg>>8);
	SPIPutChar(arg);
	SPIPutChar(0x95);	/*CRC, it must be 0x95 when cmd0*/
	
	do/*wait response*/
	{
		SPI_receive_m(&response); /*equal to "response = SPI_receive();"*/
    	if(retry++ > 0xfe) break; /*time out*/
	}while(response == 0xff); 
	
	SD_CS_DEASSERT;
	SPIPutChar(0xff);

	return response; 
}
/*-------------------------------------------------------------------------------------------------*/
unsigned char SD_readSingleBlock(unsigned char *buff, unsigned long startBlock)
{
	if(SD_sendCommand(CMD17, startBlock<<9)) return 1;

	if(receive_DataBlock(buff, 512)) return 1;

	return 0;
}
/*-------------------------------------------------------------------------------------------------*/
unsigned char SD_writeSingleBlock(unsigned char *buff, unsigned long startBlock)
{
	if(SD_sendCommand(CMD24, startBlock<<9)) return 1;
	
	if(transmit_DataBlock(buff, 0xfe)) return 1;

	return 0;
}
/*-------------------------------------------------------------------------------------------------*/
unsigned char SD_readMultipleBlock(unsigned char *buff, unsigned long startBlock, unsigned long totalBlocks)
{
	unsigned char response = 0xff;
	unsigned int retry = 0;
	unsigned int nbyte = 512;

	if(SD_sendCommand(CMD18, startBlock<<9)) return 1;

	SD_CS_ASSERT;

	do 
	{	
		do /*wait for start block token 0xfe (0x11111110)*/
		{
			SPI_receive_m(&response);
    		if(retry++ > 0xfffe) /*time out error*/
			{
				SD_CS_DEASSERT; 
				return 1;	
			}
		}while(response != 0xfe); /*wait response*/

		do /*Receive the data block into buffer*/
		{							
			SPI_receive_m(buff++);
			SPI_receive_m(buff++);
			SPI_receive_m(buff++);
			SPI_receive_m(buff++);
		}while (nbyte -= 4);

		SPIGetChar(); /*Discard CRC*/
		SPIGetChar();

		nbyte = 512;
		retry = 0;

	}while (--totalBlocks);

	SD_sendCommand(CMD12, 0); /*STOP_TRANSMISSION*/

	SD_CS_DEASSERT;
	SPIPutChar(0xff); /*extra 8 clk*/

	return 0;
}

/*-------------------------------------------------------------------------------------------------*/
unsigned char SD_writeMultipleBlock(unsigned char *buff, unsigned long startBlock, unsigned long totalBlocks)
{
	unsigned char response = 0xff; /*response from sd card*/
	unsigned char bc = 0; 
	unsigned int retry = 0;

	if(SD_sendCommand(CMD25, startBlock<<9)) return 1;

	SD_CS_ASSERT;

	do 
	{
		SPIPutChar(0xfc); /*Send start block token 0xfc (0x11111100)*/
   
		do /*ransmit the 512 byte data block to SDC*/
		{						
			SPIPutChar(*buff++);
			SPIPutChar(*buff++);
		} while (--bc);

		SPIPutChar(0xFF); /* CRC (Dummy) */
		SPIPutChar(0xFF);
			
		SPI_receive_m(&response);
		if( (response & 0x1f) != 0x05) /*response= 0bXXX0AAA1 ; AAA='010' - data accepted*/
		{                              /*AAA='101'-data rejected due to CRC error*/
 			 SD_CS_DEASSERT;           /*AAA='110'-data rejected due to write error*/
 			 return 1;
		}

		retry = 0;
		while(SPIGetChar()!=0xff) /*wait for SD card to complete writing and get idle*/
			if(retry++ > 0xfffe){SD_CS_DEASSERT; return 1;}
		SPIPutChar(0xff); /*extra 8 bits*/

	}while (--totalBlocks);

	SPIPutChar(0xfd); /*send 'stop transmission token'*/

	while(SPIGetChar()!=0xff) //wait for SD card to complete writing and get idle
   		if(retry++ > 0xfffe){SD_CS_DEASSERT; return 1;}

	SD_CS_DEASSERT;
	SPIPutChar(0xff);

	return 0;
}
/*-------------------------------------------------------------------------------------------------*/
unsigned char SD_erase(unsigned long startBlock, unsigned long totalBlocks)
{
	unsigned char response;

	response = SD_sendCommand(CMD32, startBlock<<9); /*send starting block address*/
	if(response != 0x00) /*check for SD status: 0x00 is OK*/
  		return response;

	response = SD_sendCommand(CMD33,(startBlock + totalBlocks - 1)<<9); /*send end block address*/
	if(response != 0x00)
  		return response;

	response = SD_sendCommand(CMD38, 0); /*erase all selected blocks*/
	if(response != 0x00)
  		return response;

	return 0; /*normal return*/
}
/*-------------------------------------------------------------------------------------------------*/
unsigned char receive_DataBlock(
	unsigned char *buff, /* Data buffer to store received data */ 
	unsigned int btr /* Byte count (must be multiple of 4) */
)
{
	unsigned char response = 0xff;
	unsigned int retry = 0;

	SD_CS_ASSERT;
	do /*wait for start block token 0xfe (0x11111110)*/
	{
		SPI_receive_m(&response);
    	if(retry++ > 0xfffe) /*time out error*/
		{
			SD_CS_DEASSERT; 
			return 1;	
		}
	}while(response != 0xfe); /*wait response*/

	do /*Receive the data block into buffer*/
	{								
		SPI_receive_m(buff++);
		SPI_receive_m(buff++);
		SPI_receive_m(buff++);
		SPI_receive_m(buff++);
	}while(btr -= 4);

	SPIGetChar(); /*Discard CRC two bytes*/
	SPIGetChar();

	SD_CS_DEASSERT;
	SPIPutChar(0xff); /*extra 8 clk*/

	return 0;
}
/*-------------------------------------------------------------------------------------------------*/
unsigned char transmit_DataBlock(
	const unsigned char *buff, /* 512 byte data block to be transmitted */
	unsigned char token /* Data/Stop token */
)
{
	unsigned char response = 0xff; 
	unsigned char bc = 0;
	unsigned int retry = 0;

	SD_CS_ASSERT;
	
	SPIPutChar(token); /*send start block token 0xfe (0x11111110)*/

	do /*transmit the 512 byte data block to SD card*/
	{					
		SPIPutChar(*buff++);
		SPIPutChar(*buff++);
	}while (--bc);

	SPIPutChar(0xff); /*CRC (Dummy)*/
	SPIPutChar(0xff);

	SPI_receive_m(&response);
	if( (response & 0x1f) != 0x05) /*response= 0bXXX0AAA1 ; AAA='010' - data accepted*/
	{                              /*AAA='101'-data rejected due to CRC error*/
 		 SD_CS_DEASSERT;           /*AAA='110'-data rejected due to write error*/
 		 return 1;
	}

	while(SPIGetChar()!=0xff) //wait until the SD is not busy
   		if(retry++ > 0xfffe){SD_CS_DEASSERT; return 1;}

	SD_CS_DEASSERT;
	SPIPutChar(0xff); /*extra 8 clk*/

	return 0;
}
/*-------------------------------------------------------------------------------------------------*/