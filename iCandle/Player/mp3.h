//File: mp3.h
//Top layer of music player impementations

#ifndef _MP3_H_
#define _MP3_H_


FRESULT scan_files (char* path, U16 *total_tracks);

FRESULT scan_files_open (FIL* p_file, char* path, int song_number);


#endif
