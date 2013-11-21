//file: player.c

#include "../HAL/HAL.h"
#include "../VS10xx/VS10xx.h"
#include "../UI/console.h"
#include "../UI/LCD.h"
#include "../FatFs/diskio.h"
#include "../FatFs/ff.h"
#include "player.h"
#include "mp3.h"

PLAYER Player;

U8 DataBuf[BUFSIZE];
U8 * DataPtr;
U16 BufCnt;


static data unsigned char potentiometer = 170, potentiometer_temp;

//Get system event
EVENT GetEvent()
{
	U8 key;

	if(Player.Event != EV_NULL)		//Already got a pending event?
		return Player.Event;

	if((Player.Status == PS_PLAYING) && !VS_IsBusy())
		return EV_DREQ;

	key = GetKey();
	if(key)		//key events
	{
		if((Player.Status == PS_PLAYING) && (key == KEY_PAUSE))
			return EV_PAUSE;

		if(key == KEY_PAUSE)
			return EV_PLAY;

		if(key == KEY_MODE)
			return EV_MODE;

		if(Player.Mode == SM_SONG)
		{
			if(key == KEY_NEXT)
				return EV_NEXT;
			if(key == KEY_PREVIOUS)
				return EV_PREVIOUS;
		}

		if(Player.Mode == SM_BASS)
		{
			if((key/*&0x7F*/) == KEY_PREVIOUS)	//regardless of whether the key is held down or released
			{							//Turn up
				if(Player.Bass < 15)
				{
					Player.Bass++;
					return EV_BASSTREB;
				}
			}
			if((key/*&0x7F*/) == KEY_NEXT)
			{
				if(Player.Bass > 0)
				{
					Player.Bass--;
					return EV_BASSTREB;
				}
			}
		}

		if(Player.Mode == SM_TREBLE)
		{
			if((key/*&0x7F*/) == KEY_PREVIOUS)	//turn up
			{
				if(Player.Treble < 7)
				{
					Player.Treble++;
					return EV_BASSTREB;
				}
			}
			if((key/*&0x7F*/) == KEY_NEXT)	//turn down
			{
				if(Player.Treble > -8)
				{
					Player.Treble--;
					return EV_BASSTREB;
				}
			}
		}
	}//if(key)
	potentiometer_temp = ADC0/64+50;
	potentiometer = 2*potentiometer_temp;
	if (potentiometer != Player.Volume)
	{
		Player.Volume = potentiometer;
		return EV_VOLUME;
	}
	return EV_NULL;
}

void HandleEvent()
{
	U8 RefrashCounter;
	EVENT Evt = Player.Event;

	ClearEvent();
	switch(Evt)
	{
		case EV_NULL:
			break;

		case EV_DREQ:
			if(BufCnt>=32)
			{
				VS_Send32Byte(DataPtr);
				DataPtr += 32;
				BufCnt -= 32;
			}
			else
				GenerateEvent(EV_BUFEMPTY);
			break;

		case EV_BUFEMPTY:
			if(f_eof(Player.currFile))
				GenerateEvent(EV_END);
			else
			{
				f_read((Player.currFile), DataBuf, BUFSIZE, &BufCnt);
				DataPtr = DataBuf;
			}
			RefrashCounter++;
			if(RefrashCounter == Refrash){
			LCDClean();
			LCDPutString("Now Playing #");
			LCDPutUInt(Player.SongNum);}
			break;

		case EV_NEXT:
			VS_CancelDecoding();
			if(++Player.SongNum  > Player.TotalSongNum)
				Player.SongNum= 1;
			scan_files_open (Player.currFile,"/",Player.SongNum);
			GenerateEvent(EV_BUFEMPTY);
			RefrashCounter = Reset;
			break;

		case EV_PREVIOUS:
			VS_CancelDecoding();
			if(--Player.SongNum  == 0)
				Player.SongNum= Player.TotalSongNum;
			scan_files_open (Player.currFile,"/",Player.SongNum);
			GenerateEvent(EV_BUFEMPTY);
			RefrashCounter = Reset;
			break;

		case EV_MODE:
			Player.Mode++;
			Player.Mode %= 3;
			SetLED(Player.Mode);
			LCDClean();
			LCDPutString("Mode: ");
			if(Player.Mode==SM_BASS)
			LCDPutString("Bass");
			if(Player.Mode==SM_TREBLE)
			LCDPutString("Treble");
			if(Player.Mode==SM_SONG)
			LCDPutString("Song");
			RefrashCounter = Reset;
			break;

		case EV_BASSTREB:
			VS_SetBassTreble(Player.Bass,Player.Treble);
			LCDClean();
			LCDPutString("Bass: ");
			LCDPutUInt(Player.Bass);
			if(Player.Mode==SM_BASS)
			LCDPutString(" <--");
			LCDChangeLine();
			LCDPutString("Treble: ");
			LCDPutInt(Player.Treble);
			if(Player.Mode==SM_TREBLE)
			LCDPutString(" <--");
			RefrashCounter = Reset;
			break;

		case EV_VOLUME:
			VS_SetVol(Player.Volume);
			LCDClean();
			LCDPutString("Now Playing #");
			LCDPutUInt(Player.SongNum);
			LCDChangeLine();
			LCDPutString("Volume: ");
			LCDPutUInt(((ADC0+48)/41));//0~100
			LCD_DataWrite('%');
			RefrashCounter = Reset;
			break;

		case EV_PAUSE:
//			ConsoleWrite("Paused.\n");
			Player.Status = PS_PAUSED;
			LCDClean();
			LCDPutString("|| Paused....");
			break;

		case EV_PLAY:
//			ConsoleWrite("Resume Playing...\n");
			Player.Status = PS_PLAYING;
			LCDClean();
			LCDPutString("Now Playing: #");
			LCDPutUInt(Player.SongNum);
			break;

		case EV_END:
			Player.Event = EV_NEXT;
			LCDClean();
			LCDPutString("End.");
			break;

		default:
			break;
	}
}

/********************************************
 *         Main                             *
 ********************************************/

void main()
{
	FATFS fs;
	FIL file;

	InitMCU();
	//VS_SinTest(0x74);
	VS_Init();

	if(disk_initialize(0))
		ConsoleWrite("Disk Initialization FAILED. :(\n");
	else
		ConsoleWrite("Disk Initialization Complete.\n");

	if(f_mount(0,&fs))
		ConsoleWrite("Mount FieSystem Failed\n");
	else
		ConsoleWrite("Mount FileSystem Success!\n");

	ConsoleWrite("Scaning Music Files...\n\n");
	if(scan_files("/",&(Player.TotalSongNum)))
		ConsoleWrite("Scan Files Failed\n");
	else{
		ConsoleWrite("\nScan Files Accomplished.\ntotal files: ");
		ConsolePutUInt (Player.TotalSongNum);
		ConsoleWrite ("\n\n");}

	if(scan_files_open (&file,"/",1))       // Start node to be scanned and opened
		ConsoleWrite("Open File Error\n");  //Playing mp3/track001.mp3 ... will apear in function		

	
	Player.currFile	= &file;
	Player.SongNum	= 1;
	Player.Status	= PS_PLAYING;
	Player.Mode		= SM_SONG;
	Player.Volume	= 170;
	Player.Bass		= 7;
	Player.Treble	= 0;

	VS_SetBassTreble(Player.Bass,Player.Treble);

	BufCnt = 0;
//	GenerateEvent(EV_BUFEMPTY);

	//Main loop
	while(1)
	{
		//Get system event
		Player.Event = GetEvent();
		//Handle Events
		HandleEvent();
	}
}
