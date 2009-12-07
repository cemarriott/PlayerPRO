/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/

#ifndef __AMFH__
#define __AMFH__

#if defined(powerc) || defined(__powerc) || defined(__APPLE__)
#pragma options align=mac68k
#endif

#define uchar unsigned char
#define ushort unsigned short
#define ulong unsigned long

typedef struct {
    uchar       type;
    char        name[32],filename[13];
    void        *sample;
    ushort      size;
    ushort      rate;
    uchar       volume;
    ushort      loopstart,loopend;
} OLDINSTRUMENT;

typedef struct {
    uchar       type;
    char        name[32],filename[13];
    void        *sample;
    ulong       size;
    ushort      rate;
    uchar       volume;
    ulong       loopstart,loopend;
} INSTRUMENT;
#if defined(powerc) || defined(__powerc) || defined(__APPLE__)
#pragma options align=reset
#endif
#endif
