//file VS10xx.c
//operations to vs10xx chip


#include "../HAL/HAL.h"
#include "VS10xx.h"

#define NO_DEBUG
#include "../UI/console.h"


#define VS_SelectData()			Mp3SelectData()//{ VS_XDCS = 0; }
#define VS_DeselectData()		Mp3DeselectData()//{ VS_XDCS = 1; }

#define VS_SelectControl()		Mp3SelectControl()//{ VS_XCS = 0; }
#define VS_DeselectControl()	Mp3DeselectControl()//{ VS_XCS = 1; }

#define VS_PutInReset()			Mp3PutInReset()//{ VS_XRESET = 0; }
#define VS_ReleaseFromReset()	Mp3ReleaseFromReset()//{ VS_XRESET =1; }

#define VS_WriteByte(x) SPIPutChar(x);//SPI_ReadWriteByte(x)
#define VS_ReadByte()	SPIGetChar();//SPI_ReadWriteByte(0xff)


//Description:
//	Write Register of VS10xx at Address [addr]
//Param:
//	addr:Address; dat:Value to write
void VS_WriteReg(U8 addr,U16 dat)
{  
    while(VS_IsBusy()) {}//wait for VS10xx DREQ
    //XCS low, enable SCI read/write
    VS_SelectControl();
	VS_WriteByte(VS_WRITE_COMMAND);  	//indicates starting to write
    VS_WriteByte(addr);  	//Address
    VS_WriteByte(dat>>8);  	//Higher byte
    VS_WriteByte(dat);     	//Lower byte
	SPIWait();   
	//XCS high, disable SCI
	VS_DeselectControl();
}

//Description:
//	Read Register of VS10xx at addr
//Param:
//	addr	Address
U16 VS_ReadReg(U8 addr)
{
	U16 ret;	//return value
	while(VS_IsBusy()) {}//wait for DREQ
    //Enable SCI, as VS_WriteReg()
    VS_SelectControl();
	VS_WriteByte(VS_READ_COMMAND);  	//indicates starting to read
    VS_WriteByte(addr);  	//address
	ret = VS_ReadByte();		//Higher byte
	ret <<= 8;
	ret += VS_ReadByte();		//lower byte   
	//Disable SCI, as VS_WriteReg()
	SPIWait();
	VS_DeselectControl();
	return ret;
}

//Description:
//	Set volume 0~255
void VS_SetVol(U8 vol)
{
	UU16 temp_vol;
	temp_vol.U8[MSB] = 255 - vol;
   	temp_vol.U8[LSB] = 255 - vol;
    VS_WriteReg(SCI_VOL,temp_vol.U16); //write to register
}

/*
//Description:
//	Set BASS 0~255 
void VS_SetBass(unsigned char bass)
{
	unsigned int temp_bass=0;
	bass=255-bass;
   	temp_bass=bass*256+bass;
    VS_WriteReg(SCI_BASS,temp_bass); //write to register
}
*/

//description
//VS1053 Set Bass 0~15, and Treble -8~7
void VS_SetBassTreble(U8 bass, char treble)
{
	UU16 tone;
	bass &= 0x0F;
	treble &= 0x0F;

	tone.U8[MSB] = (treble<<4)|TREB_FREQ;
	tone.U8[LSB] = (bass<<4)|BASS_FREQ;

	//ConsolePutHex16(tone.U16);

	VS_WriteReg(SCI_BASS,tone.U16);
}


//Description:
//	VS10xx Initialization
void VS_Init(void)
{
	SPISetSlowClock();			//Set Slow Clock to do the initialization
	Delay(10);
	
	//ensure the chip select pins are high
	VS_DeselectControl();	//SCI
	VS_DeselectData();		//SDI

	//Hardware Reset
	VS_ReleaseFromReset(); 
	Delay(1);
	VS_PutInReset();
	Delay(1);
	VS_ReleaseFromReset();
	Delay(1);

	while(VS_IsBusy());

	VS_SoftReset();
	VS_SetVol(192);	 			//Initial Volume
	//VS_SetBass(200); 			//Initial Bass
	//enable SDI
	VS_SelectData();
	VS_WriteByte(0);    		//Write NULL bytes to start up
	VS_WriteByte(0);
	VS_WriteByte(0);
	VS_WriteByte(0);
	SPIWait();
	//Disable SDI
	VS_DeselectData();
	SPISetFastClock();			//Turn back to High Speed
	Delay(10);
} 

//VS10xx sine test
void VS_SinTest(unsigned char x)
{
	//Hardware reset
	VS_PutInReset();
	Delay(100);
	SPIPutCharWithoutWaiting(0xFF);
	VS_ReleaseFromReset();
	Delay(100);            
	VS_WriteReg(0x00,0x0820);//start test 0x0820   SM_SDINEW为1   SM_TEST为1
	while(MP3_DREQ==0);   	//wait for DREQ
	//VS_XDCS=0;	    		//enable SDI
	VS_SelectData()	;
	VS_WriteByte(0x53);		//write this sequence of bytes to start test
	VS_WriteByte(0xef); 
	VS_WriteByte(0x6e);
	VS_WriteByte(x);  		//x to set frequencies   FsIdx (b7~b5):Sample Rate Index; S(b4~b0):Sine skip speed; frequency F=Fs * S / 128
	VS_WriteByte(0);   		//e.g. x=126 (0b 011 11110) FsIdx=011=3   Fs=22050Hz   S=11110=30    F=22050Hz X 30 /128 =5168 Hz
	VS_WriteByte(0);
	VS_WriteByte(0);
	VS_WriteByte(0); 
	SPIWait(); 
	Delay(1000);			//wait for a while 
	VS_WriteByte(0x45);		//write this sequence of bytes to exit testing
	VS_WriteByte(0x78); 
	VS_WriteByte(0x69);
	VS_WriteByte(0x74);
	VS_WriteByte(0);
	VS_WriteByte(0);
	VS_WriteByte(0);			   
	VS_WriteByte(0);
	SPIWait();
	//VS_XDCS=1;	    		//disable SDI
	VS_DeselectData();
} 


//VS10xx soft reset
void VS_SoftReset()
{
	VS_WriteReg(SCI_MODE,0x0804);	//software reset
	Delay(10);
	while(VS_IsBusy());
	//set clock frequency, 
	VS_WriteReg(SCI_CLOCKF,SCI_ClockF(4, 3, 0));
	Delay(1);
	while(VS_IsBusy());
}


void VS_Send32Byte(U8* buf)
{
	U8 i;
	VS_SelectData();
	while(!MP3_DREQ);
	for(i=0;i<32;i++)
		VS_WriteByte(buf[i]);
	SPIWait();
	VS_DeselectData();
}
/*
//Description:
//	Send multiple bytes of datat to VS10xx
void VS_SendData(U8* buf, U8 size)
{
	VS_SelectData();
	while(size >= 32)
	{
		while(VS_IsBusy());

		SPI_WriteMultiBytes(buf, 32);
		size -= 32;
	}
	if(size > 0)
	{
		while(VS_IsBusy());
		SPI_WriteMultiBytes(buf, size);
	}
	VS_DeselectData();
}
*/
//Description:
// Send 2048 zeros to clear FIFO
void VS_Fill2048Zero()
{
	unsigned char i,j;

	for(i=0;i<64;i++)
	{
		while(VS_IsBusy());
		VS_SelectData();

		for(j=0;j<32;j++)
		{
			VS_WriteByte(0);
		}
		SPIWait();
		VS_DeselectData();
	}
	while(VS_IsBusy());
}

#if MP3_REC
//说明：
//	进入ADPCM 录音模式
//参数：
//	highPass是否高通滤波，如果高能滤波，对一些背景音会有抑制效果。
//	select_in	选择输入
//				VS_MICROPHONE	
//				VS_LINE_IN	
//备注：
//	1.	录音的内容可以在 SCI_HDAT0 寄存器中读到！
//	2.	在录音文件前面需要有个header
//	3.	现在只支持8k的采样率
//	4.	现在只支持AGC(automatic gain control)模式
void VS_ADPCMRecordingMode(BOOL highPass, U8 select_in)
{
	U16 temp;
	// Before activating ADPCM recording, user should write a clock divider value to SCIAICTRL0.
	//Fc = 3.0 x 12:288MHz, d= 18. Now fs = (3.0 x 12288000)/( 256*18) = 8kHz (sample rate)
	VS_WriteReg(SCI_AICTRL0, 12);
	//Also, before activating ADPCM mode, the user has to set linear recording gain control to register SCIAICTRL1
	VS_WriteReg(SCI_AICTRL1, 0);	//automatic gain control	

	temp = VS_ReadReg(SCI_MODE);
	temp |= (0x0004|0x1000);//= SetBit(U16, temp, Bit(2) | Bit(12));
	if(highPass)
	{
		temp |= 0x2000;//= SetBit(U16, temp, Bit(13));
	}
	else
	{
		temp &= (0xffff-0x2000);//ClearBit(U16, temp, Bit(13));
	}
	if(select_in == VS_MICROPHONE)
	{
		temp &= (0xffff-0x4000);//= ClearBit(U16, temp, Bit(14));
	}
	else
	{
		temp |= 0x4000;//= SetBit(U16, temp, Bit(14));
	}
	//时钟设置，向3号寄存器写入0x9800   SC_MULT  为2   SC_ADD  为3   SC_FREQ为0
	VS_WriteReg(SCI_CLOCKF,SCI_ClockF(2, 3, 0));
	VS_WriteReg(SCI_MODE, temp);
	return;
}
#endif //MP3_REC


//VS_RecordInit();


#ifdef VS_TEST
void VS_Test()
{
	U16 temp;
	DB_SendString("VS_ReadReg(0x04) :");
	DB_SendDec(VS_ReadReg(0x04));
	DB_SendString("\nVS_ReadReg(0x00) :");
	DB_SendDec(VS_ReadReg(0x00));
	DB_SendString("\nvs read in normal mode :");
	
	for(temp = 64; temp > 0; temp--)
	{
		ConsolePutHex16(VS_ReadReg(SCI_HDAT0));
	}

	VS_ADPCMRecordingMode(0, 0);

//	DB_SendString("\nVS_ReadReg(0x00) :");
//	DB_SendDec(VS_ReadReg(0x00));
//	DB_SendString("\nvs read in adpcm mode :");
//	for(temp = 512; temp > 0; temp--)
//	{
//		DB_SendHex(VS_ReadReg(SCI_HDAT0));
//	}
	while( VS_ReadReg(SCI_HDAT1) < 256 )
	{
		SendASC('.');
	};
	for(temp = 512; temp > 0; temp--)
	{
		VS_ReadReg(SCI_HDAT0);
	}
	for(temp = 0xffff; temp; temp--);
	for(temp = 0xffff; temp; temp--);
	for(temp = 0x8fff; temp; temp--);
	temp = VS_ReadReg(SCI_HDAT1);
	ConsoleWrite("\nNow Read sci_hdat1 :");
	ConsolePutUInt(temp);
	while(1);
}
#endif

#if 0
/*
Definitions from datasheet
SDI: 
	serial data interface, The serial data interface is meant for 
	transferring compressed MP3 or WMA data, WAV PCM and AD-PCM data
	as well as MIDI data.
SCI: 
	serial control interface, The serial control interface is 
	compatible with the SPI bus specification. Data transfers are always 16
	bits. VS1003 is controlled by writing and reading the registers of the interface.
CLKI:
	internal clock.
*/
#endif

