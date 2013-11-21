//-----------------------------------------------------------------------------
// TS_vInterface.h
//-----------------------------------------------------------------------------
// Copyright 2007 Silicon Laboratories, Inc.
// http://www.silabs.com
//
// Program Description:
//
// This file declares the interface functions for the ToolStick Virtual 
// Tools. All the interface library functions (except the SilabsInit020 function) 
// can be used with all Silicon Labs MCUs as long as the UART0 is setup for 
// 230400 bps, 8-N-1. For devices with SFR Paging, the SFR page should be set to
// the UART0 page before calling any of the interface library functions. The only 
// SFRs used by the interface functions (except the SilabsInit020 function) are 
// SBUF0, TI0, and RI0, which are located at the same SFR locations in all
// Silicon Labs MCUs.
//
// Target:         C8051F020 (SilabsInit020), All Silicon Labs MCUs (others)
// Tool chain:     Generic
// Command Line:   None
//
// Release 1.0
//    -14 SEP 2007 (PKC)
//    -Added LIB_VERSION_NUMBER and Lib_VersionRead
// Release 0.1
//    -Initial Revision (DM)
//    -23 OCT 2005

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#ifndef TS_VINTERFACE_H
#define TS_VINTERFACE_H

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
enum { OFF, ON };                      // ON and OFF constants (OFF==0, ON==1)

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void SilabsInit020 (void);             // Initialize hardware and variables
                                       // Only compatible with C8051F02x MCUs

void LCD_ControlWrite (U8);            // Send control character to LCD
U8 LCD_ControlRead (void);             // Read control character from LCD
void LCD_DataWrite (U8);               // Send data character to LCD
U8 LCD_DataRead (void);                // Read data character from LCD
void ScopeSampleWrite (U8, U8, U8);    // Write a sample to oscilloscope
void ScopeClearBuffer (U8);            // Resets the oscilloscope buffer
void TerminalWrite (U8);               // Send data character to vTerm
U8 TerminalRead (void);                // Read data character from vTerm
U8 Lib_VersionRead (void);             // Read the interface library version

//-----------------------------------------------------------------------------
// Header File Preprocessor Directive
//-----------------------------------------------------------------------------
#endif                                 // TS_vInterface.h

//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------