//file: mp3.c

#include "../HAL/HAL.h"	
#include "../UI/console.h"
#include "../UI/LCD.h"
#include "../FatFs/ff.h"
//#include "../VS10xx/VS10xx.h"
#include "mp3.h"


FRESULT scan_files (
    char* path,        /* Start node to be scanned (also used as work area) */
	U16 *total_tracks
)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;

    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif

	(*total_tracks)=0;
    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {

        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (!(fno.fattrib & AM_DIR)) {			/* It is a file. */
				(*total_tracks)++;
				ConsolePutUInt (*total_tracks);
				ConsoleWrite(". ");
                ConsoleWrite(fn);
				ConsolePutChar('\n');
            }
        }
    }

    return res;
}

FRESULT scan_files_open (
    FIL* p_file,
	char* path,			 /* Start node to be scanned (also used as work area) */
	int song_number
)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        //i = strlen(path);
        for (i=0;i!=song_number;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (!(fno.fattrib & AM_DIR)) {                 /* It is a file. */
				i++;
            }
        }

		f_close(p_file);	//close the file opened before
		if(f_open(p_file, fn, FA_READ))
			ConsoleWrite("Open File Error\n");
		else
			ConsoleWrite("Now Playing: ");
            ConsoleWrite(fn);//printf("%s/%s\n", path, fn);
			ConsoleWrite("...\n");
			LCDClean();
			LCDPutString("Now Playing #");
			LCDPutUInt(song_number);
			LCDChangeLine();
			LCDPutString(fn);
    }

    return res;
}
