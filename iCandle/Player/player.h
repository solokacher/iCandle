//file: player.h

#ifndef _PLAYER_H_
#define _PLAYER_H_

#define VER
#define BUFSIZE 256

typedef enum{
	PS_STOPPED = 0,
	PS_PLAYING,
	PS_PAUSED
}STATUS;

typedef enum{
	EV_NULL = 0,	//Nothing happened
	EV_DREQ,		//VS10xx requires more data
	EV_BUFEMPTY,	//Buffer is empty
	EV_NEXT,		//Next song
	EV_PREVIOUS,	//Previous song
	EV_MODE,		//Mode change
	EV_BASSTREB,	//Bass/Treble adjustment
	EV_VOLUME,		//Volume adjustment
	EV_PAUSE,		//Pause
	EV_PLAY,		//Resume playing
	EV_END			//File end
}EVENT;

typedef enum{
	SM_SONG = 0,
	SM_BASS,
	SM_TREBLE
}MODE;

typedef struct{
	FIL			*currFile;
	U16			SongNum;
	U16			TotalSongNum;
	MODE		Mode;
	STATUS		Status;
	U8			Volume;
	U8			Bass;
	S8			Treble;
	EVENT		Event;
}PLAYER;

extern PLAYER Player;

#define GenerateEvent(_Evt_)  (Player.Event=_Evt_)
#define ClearEvent()		  (Player.Event=EV_NULL)
#define CleanReflash()        (reflash = 0x00)
#define Reset 0x00
#define Refrash 0xFF


void SendMoreData();



#endif