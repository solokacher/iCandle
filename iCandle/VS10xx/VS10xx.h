#ifndef VS1003_H_
#define VS1003_H_

//#include "compiler_defs.h"

#define MP3_REC 0

#define BASS_FREQ	9		//From 2 to 15, in 10Hz steps
#define TREB_FREQ	9		//From 1 to 15, in 1000Hz steps

//***********************  Commands and Addresses ***********************
#define VS_WRITE_COMMAND 	0x02
#define VS_READ_COMMAND 	0x03

//VS10xx register address
#define SCI_MODE        	0x00   
#define SCI_STATUS      	0x01   
#define SCI_BASS        	0x02   
#define SCI_CLOCKF      	0x03
#define SCI_DECODE_TIME 	0x04   
#define SCI_AUDATA      	0x05   
#define SCI_WRAM        	0x06   
#define SCI_WRAMADDR    	0x07   
#define SCI_HDAT0       	0x08   
#define SCI_HDAT1       	0x09 
  
#define SCI_AIADDR      	0x0a   
#define SCI_VOL         	0x0b   
#define SCI_AICTRL0     	0x0c   
#define SCI_AICTRL1     	0x0d   
#define SCI_AICTRL2     	0x0e   
#define SCI_AICTRL3     	0x0f   
#define SM_DIFF         	0x01   
#define SM_JUMP         	0x02   
#define SM_RESET        	0x04   
#define SM_OUTOFWAV     	0x08   
#define SM_PDOWN        	0x10   
#define SM_TESTS        	0x20   
#define SM_STREAM       	0x40   
#define SM_PLUSV        	0x80   
#define SM_DACT         	0x100   
#define SM_SDIORD       	0x200   
#define SM_SDISHARE     	0x400   
#define SM_SDINEW       	0x800   
#define SM_ADPCM        	0x1000   
#define SM_ADPCM_HP     	0x2000 

//Description:
//	Get Register SCI_CLOCKF setting value from parameters
//Param:
//	SCMULT	Clock multiplier
//	SCADD	Allowed multiplier addition
//	SCFREQ	Clock frequency. The formula for calculating the correct value for this register is
//			(XTALI - 8000000)/ 4000,(XTALI is in Hz). The default value 0 is assumed to mean XTALI=12.288 MHz.
#define SCI_ClockF(SC_MULT, SC_ADD, SC_FREQ)			(((SC_MULT)<<13) | ((SC_ADD)<<11) |(SC_FREQ))

#define VS_IsBusy()		(MP3_DREQ == 0)

void VS_WriteByte(U8 dat);
void VS_WriteReg(U8 addr,U16 dat);
U16 VS_ReadReg(U8 addr);


void VS_SoftReset();
//#define VS_SoftReset() {VS_WriteReg(SCI_MODE,0x0804);Delay(2);} //Soft reset


void VS_SendByte(unsigned char dat);
//void VS_SendData(U8* buf, U8 size);
void VS_Send32Byte(U8* buf);

void VS_Fill2048Zero();

#define VS_MICROPHONE		0	
#define VS_LINE_IN			1


#if MP3_REC
#ifndef BOOL
#define BOOL U8
#endif
void VS_ADPCMRecordingMode(BOOL highPass, U8 select_in);

#endif //MP3_REC
/*****************************/

void VS_Init(void);			//reset and initialize
void VS_SetVol(U8 vol);		//set volume
void VS_SetBassTreble(U8 bass, char treble);
void VS_SinTest(U8 x);		//sine test
#define VS_CancelDecoding()		VS_WriteReg(SCI_MODE,0x0808)

#ifdef VS_TEST
void VS_Test();
#endif

#endif
