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

#ifndef __RDRIVERH__
#define __RDRIVERH__

#ifndef __MADI__
#include "MAD.h"
#endif
#include <limits.h>

////////////////////////////////////////////////

#ifdef WIN32
#ifndef __DSOUND_INCLUDED__
#include <mmreg.h>
#include <DSound.h>
#endif
#endif

#ifdef LINUX
#include <alsa/asoundlib.h>
#endif

#ifdef _ESOUND
#include <esd.h>
#endif

#ifdef _OSSSOUND
#warning headers not included!
//TODO: include headers
#endif

////////////////////////////////////////////////

#if PRAGMA_STRUCT_ALIGN
#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
#pragma pack(2)
#endif

////////////////////////////////////////////////

/********************						***********************/
/*** 			   		 Define List							***/
/********************						***********************/

#define NUMBER_NOTES		96
#define NOFINETUNE			8363
#define MIN_VOLUME			0
#define MAX_VOLUME			64
#define MAX_CHANVOL			128
#define MAX_PANNING			64
#define MAXDRAW				10
//#define MAXTRACK			32
//#define MAXINSTRU			64
#define MAXSAMPLE			64L
#define MAXPATTERN			200
#define MAXPATTERNSIZE		900
#define MAX_ARP 			3
#define MAXPLUG				40
#define EQPACKET			512
#define MAXCHANEFFECT		20

/********************						***********************/
/*** 			  		  Error messages 						***/
/********************						***********************/

enum
{
	MADNeedMemory 					= -1,
	MADReadingErr					= -2,
	MADIncompatibleFile				= -3,
	MADLibraryNotInitialized		= -4,
	MADParametersErr				= -5,
	MADUnknowErr					= -6,
	MADSoundManagerErr				= -7,
	MADOrderNotImplemented			= -8,
	MADFileNotSupportedByThisPlug	= -9,
	MADCannotFindPlug				= -10,
	MADMusicHasNoDriver				= -11,
	MADDriverHasNoMusic				= -12,
	MADUnknownErr					= MADUnknowErr
};

/********************						***********************/
/*** 			   Channel structure definition					***/
/********************						***********************/

typedef struct Channel
{
		SInt32		ID;					// Channel ID - 0 to MAXTRACK
		SInt32		TrackID;			// TrackID - 0 to MAXTRACK (Used in multiChannel mode)
		
		char		*begPtr;			// Sample Data Ptr - Beginning of data
		char		*maxPtr;			// Sample Data Ptr - End of data
		char		*curPtr;			// Sample Data Ptr - Current position
		SInt32		sizePtr;			// Sample Size in bytes
		
		SInt32		amp;				// Sample amplitude: 8 or 16 bits
		
		SInt32		loopBeg;			// Loop Beginning
		SInt32		loopSize;			// Loop Size
		
		SInt32		ins;				// Current Instrument ID
		SInt32		insOld;				// Previous Instrument ID played on this channel
		SInt32		samp;				// Current Sample ID
		
		SInt32		fineTune;			// Finetune
		
		SInt32		note;				// Note
		SInt32		noteOld;			// Previous note played on this channel
		SInt32		relNoteOld;			// Previous realnote
		
		SInt32 		period;				// Current period
		SInt32		periodOld;			// Previous period played on this channel
		
		SInt32		vol;				// Channel vol (0 to 64)
		SInt32		pann;				// Channel pan (0 to 64)
		
		SInt32 		cmd;				// Command
		Byte		arg;				// Argument of command
		Byte		volcmd;				// Volume Command
		
		SInt32 		arp[ MAX_ARP];		// Used for arpeggio command
		SInt32 		arpindex;			// Used for arpeggio command
		Boolean		arpUse;
		
		char		viboffset;			// Used for vibrato command
		SInt32 		vibdepth;			// Used for vibrato command
		SInt32 		vibrate;			// Used for vibrato command
		SInt32		vibtype;			// Used for vibrato command
		
		SInt32 		slide;				// Used for slideUp and slideDown command
		
		SInt32 		pitchgoal;			// Used for portamento command
		SInt32 		pitchrate;			// Used for portamento command
		
		SInt32 		volumerate;			// Used for slideVolume command
		
		SInt32		oldArg[ 16];
		SInt32		oldVibrate;
		SInt32		oldVibdepth;
		
		SInt32		eventTime;
		
		char		*samplePtr;			// Used internaly by MADPlaySoundData & MADPlaySndHandle
		
		/**/
		
		Boolean		KeyOn;
		short		a;
		short		b;
		float		p;
		SInt32		volEnv;
		SInt32		nextvolEnv;
		SInt32		volEnvInter;
		
		SInt32		volFade;
		SInt32		nextvolFade;
		
		short		aa;
		short		bb;
		float		pp;
		SInt32		pannEnv;
		SInt32		nextpannEnv;
//		long		pannEnvInter;
		
		Boolean		volEnvActive, pannEnvActive;
		
		short		aaa;
		short		bbb;
		float		ppp;
		SInt32		panPitch;
		
		SInt32		lAC;
		
		char		*prevPtr;
		SInt32		lastWordL, curLastWordL;
		SInt32		lastWordR, curLastWordR;
		SInt32		curLevelL, curLevelR;
		
		Boolean		LevelDirectionL, LevelDirectionR, RemoverWorking;
		
		SInt32		prevVol0;
		SInt32		prevVol1;
		
		/**/
		
		Boolean		GEffect;
		short		GPat, GReader;
		
		/**/
		
		Boolean		stereo;
		
		Byte		loopType;
		Boolean		pingpong;
		
		SInt32		preOff;
		char		preVal, preVal2;
		char		preValR, preVal2R;
		
		short		spreVal, spreVal2;
		short		spreValR, spreVal2R;
		SInt32		TICKREMOVESIZE;
		
		
		Boolean		PanningE8;
		short		trig;
		short		PatternLoopE6, PatternLoopE6Count, PatternLoopE6ID;
		
		SInt32		TimeCounter;
} Channel;

/********************						***********************/
/*** 		Music description - used in Import/Export filter	***/
/********************						***********************/

typedef struct MADMusic
{
	MADSpec					*header;								// Music Header - See 'MAD.h'
	PatData					*partition[ MAXPATTERN];				// Patterns
	InstrData				*fid;									// Instruments
	sData					**sample;								// Samples
	FXSets					*sets;									// FXSettings
	Boolean					musicUnderModification;					// Tell the driver to NOT access music data
	Str255					musicFileName;
	Boolean					hasChanged;
	SInt32					position, fullTime;
	OSType					originalFormat;
	
} MADMusic;

/********************						***********************/
/*** 			     Driver Settings definition					***/
/********************						***********************/

enum
{
	oldASCSoundDriver DEPRECATED_ATTRIBUTE = 1,	// MAC ONLY,	// NOT Available
	oldAWACSoundDriver DEPRECATED_ATTRIBUTE,	// MAC ONLY		// NOT Available
	MIDISoundDriver,							// MAC ONLY		// NOT Available
	SoundManagerDriver DEPRECATED_ATTRIBUTE,	// MAC ONLY		// NOT Available
	QK25SoundDriver DEPRECATED_ATTRIBUTE,		// MAC ONLY		// NOT Available
	DigiDesignSoundDriver DEPRECATED_ATTRIBUTE,	// MAC ONLY		// NOT Available
	BeOSSoundDriver DEPRECATED_ATTRIBUTE,		// BE ONLY when using with BeOS compatible systems ! - NOT FOR MAC
	DirectSound95NT,							// WINDOWS 95/NT ONLY when using with PC compatible systems ! - NOT FOR MAC
	Wave95NT,									// WINDOWS 95/NT ONLY when using with PC compatible systems ! - NOT FOR MAC
	CoreAudioDriver,							// OSX ONLY Core Audio driver
	ALSADriver,									// LINUX ONLY ALSA driver
	OSSDriver,									// Open Sound System. Most Unices (NOT OS X) including Linux
	ESDDriver,									// ESound Driver. available on most UNIX Systems
	ASIOSoundManager DEPRECATED_ATTRIBUTE,		// ASIO Sound Driver by Steinberg //NOT Available
	NoHardwareDriver = SHRT_MAX					// NO HARDWARE CONNECTION, will not produce any sound

};

enum
{
	oldMonoOutPut = 1,				// NOT SUPPORTED anymore
	oldStereoOutPut,				// NOT SUPPORTED anymore
	DeluxeStereoOutPut,				// USE THIS ONE ONLY !!!!!!!!!!!!!!
	PolyPhonic,						// Do NOT use it ! Standard hardware doesn't support it !
	
	StereoOutPut = oldStereoOutPut,
	MonoOutPut = oldMonoOutPut
};

typedef struct MADDriverSettings
{
	short					numChn;								// Active tracks from 2 to 32, automatically setup when a new music is loaded
	short					outPutBits;							// 8 or 16 Bits TODO: 24 Bits
	double					outPutRate;							// Fixed number, by example : rate44khz, rate22050hz, rate22khz, rate11khz, rate11025hz
	short					outPutMode;							// Now, only DeluxeStereoOutPut is available !
	short					driverMode;							// MIDISoundDriver, SoundManagerDriver, BeOSSoundDriver, DirectSound95NT or Wave95NT
//	Boolean					antiAliasing;						// NOT USED anymore
	Boolean					repeatMusic;						// If music finished, repeat it or stop.
//	Boolean					sysMemory;							// NOT USED anymore
//	Boolean					Interpolation;						// NOT USED anymore
//	Boolean					MicroDelay;							// NOT USED anymore
	SInt32					MicroDelaySize;						// Micro delay duration (in ms, max 1 sec = 1000 ms, min = 0 ms)
	Boolean					surround;							// Surround effect active? true/false
	Boolean					Reverb;								// Reverb effect active? true/false
	SInt32					ReverbSize;							// Reverb delay duration (in ms, min = 25 ms, max 1 sec = 1000 ms)
	SInt32					ReverbStrength;						// Reverb strength in % (0 <-> 70)
	Boolean					TickRemover;						// Remove volume/sample/loop ticks.
	SInt32					oversampling;						// OverSampling value, 1 = normal; works ONLY on 64bits processor (PowerPC)
} MADDriverSettings;

/******************************************************************/
//******************* MUSICS IMPORT/EXPORT PLUGS  *****************/
//
//	Protocol Version 2.3
//
//	To use with PlayerPRO for CodeWarrior
//
//	Your main function have to be in this form:
//	OSErr PPImpExpMain(	OSType order,
//						char *AlienFileName,
//						MADMusic *MadFile,
//						PPInfoRec *info,
//						MADDriverSettings *DriverParam);
//
//	Actual plug have to support these orders:
//
//	order: 'TEST':	check the AlienFile to see if your Plug really supports it.
//	order: 'IMPT':	convert the AlienFile into a MADMusic. You have to allocate MADMusic.
//	order: 'INFO':	Fill PPInfoRec structure.
//	order: 'EXPT':	Convert the MADMusic into AlienFile. You have to create the AlienFile.
//					Don't delete the MADMusic Structure after conversion !!
//
//	An IMPORT plug have to support these orders: 'TEST', 'IMPT', 'INFO'
//	An EXPORT plug have to support these orders: 'EXPT'
// 	An IMPORT/EXPORT plug have to support these orders: 'TEST', 'IMPT', 'INFO', 'EXPT'
//
//
/********************						***********************/

typedef struct PPInfoRec
{
	char		internalFileName[ 60];
	char		formatDescription[ 60];
	
	SInt32		totalPatterns;
	SInt32		partitionLength;
	
	short		totalTracks;
	short		totalInstruments;
	
	OSType		signature;
	
	long		fileSize;
	
} PPInfoRec;


/********************						***********************/
/*** 			Informations about Plugs: ThePlug[]				***/
/********************						***********************/

#ifdef _MAC_H

typedef OSErr (*MADPLUGFUNC) ( OSType , Ptr , MADMusic* , PPInfoRec *, MADDriverSettings *);

typedef struct PlugInfo
{
	MADPLUGFUNC	IOPlug;											// Plug CODE
	CFStringRef	MenuName;										// Plug name
	CFStringRef	AuthorString;									// Plug author
	CFBundleRef	file;											// Location of plug file
	char		type[ 5];										// OSType of file support. Kept for legacy reasons.
	CFArrayRef	UTItypes;										// CFStrings of supported UTIs
	OSType		mode;											// Mode support : Import +/ Export
} PlugInfo;
#endif

#ifdef WIN32
#include <windows.h>
typedef OSErr (*PLUGDLLFUNC) ( OSType , Ptr , MADMusic* , PPInfoRec *, MADDriverSettings *);
typedef struct PlugInfo
{
	HANDLE			hLibrary;
	PLUGDLLFUNC		IOPlug;										// Plug CODE
	char			MenuName[ 65];								// Plug name
	char			AuthorString[ 65];							// Plug author
	char			file[ 255];									// Location of plug file
	char			type[ 5];									// OSType of file support
	OSType			mode;										// Mode support : Import +/ Export
} PlugInfo;
#endif

#if (defined(__UNIX__) && !defined (_MAC_H))
#include <dlfcn.h>
#include <sys/param.h>  //For PATH_MAX
typedef OSErr (*MADPLUGFUNC) ( OSType , Ptr , MADMusic* , PPInfoRec *, MADDriverSettings *);
typedef struct PlugInfo
{
	void*			hLibrary;
	MADPLUGFUNC		IOPlug;										// Plug CODE
	char			MenuName[ 65];								// Plug name
	char			AuthorString[ 65];							// Plug author
	char			file[PATH_MAX];								// Location of plug file
	char			type[ 5];									// OSType of file support
	OSType			mode;										// Mode support : Import +/ Export
} PlugInfo;
#endif


/********************						***********************/
/*** 		Global structure : PlayerPRO variables				***/
/********************						***********************/

typedef struct MADLibrary
{
	OSType					IDType;								// IDType = 'MADD' -- READ ONLY --
	SInt32					mytab[ 12];
	
	/** Plugs Import/Export variables **/
	
	PlugInfo 				*ThePlug;							// Pointers on plugs code & infos
	short					TotalPlug;							// no of Plugs in pointer ThePlug

} MADLibrary;

#ifndef WIN32
#define callback
#endif

typedef struct AEffect AEffect;

struct AEffect
{
	SInt32 magic;
	SInt32 (callback *dispatcher)(AEffect *effect, SInt32 opCode, SInt32 index, SInt32 value,
		void *ptr, float opt);
	void (callback *process)(AEffect *effect, float **inputs, float **outputs, SInt32 sampleframes);
	void (callback *setParameter)(AEffect *effect, SInt32 index, float parameter);
	float (callback *getParameter)(AEffect *effect, SInt32 index);

	SInt32 numPrograms;
	SInt32 numParams;
	SInt32 numInputs;
	SInt32 numOutputs;
	SInt32 flags;
	SInt32 resvd1;
	SInt32 resvd2;
	SInt32 initialDelay;
	SInt32 realQualities;
	SInt32 offQualities;
	float ioRatio;
	void *object;
	void *user;
	SInt32 uniqueID;
	SInt32 version;
	void (callback *processReplacing)(AEffect *effect, float **inputs, float **outputs, SInt32 sampleframes);
	char future[60];
};

typedef	SInt32 (*audioMasterCallback)(AEffect *effect, SInt32 opcode, SInt32 index,
		SInt32 value, void *ptr, float opt);
	
	typedef AEffect *(*VSTPlugInPtr) (audioMasterCallback  cb);

#ifdef _MAC_H
//TODO: use OS X's native VST?
typedef struct
{
	AEffect				*ce[ 2];
	short				VSTid;
	CFStringRef			name;
	Boolean				Active;
	CFBundleRef			connID; //TODO: use something more 64-bit friendly
	VSTPlugInPtr		vstMain;
	Boolean				ProcessReplacingNotAvailable;

}	VSTEffect;
#endif

typedef struct __MADDriverRec MADDriverRec;

/********************						***********************/
/*** 					   EFFECTS ID							***/
/********************						***********************/

enum {
		arpeggioE 		= 0,	//	0x00
		downslideE 		= 1,	//	0x01
		upslideE 		= 2,	//	0x02
		portamentoE 	= 3,	//	0x03
		vibratoE 		= 4,	//	0x04
		portaslideE 	= 5,	//	0x05
		vibratoslideE	= 6,	//	0x06
		nothingE 		= 7,	//	0x07
		panningE		= 8,	//	0x08
		offsetE 		= 9,	//	0x09
		slidevolE 		= 10,	//	0x0A
		fastskipE 		= 11,	//	0x0B
		volumeE 		= 12,	//	0x0C
		skipE 			= 13,	//	0x0D
		extendedE 		= 14,	//	0x0E
		speedE 			= 15,	//	0x0F
		noteoffE 		= 16,	//	0x0G
		LoopE			= 17,	// 	0x0L
		NOffSetE		= 18	//  0x0O
	};

/********************						***********************/
/*** 					   FUNCTIONS							***/
/********************						***********************/


#ifdef __cplusplus
extern "C" {
#endif

void MyDebugStr( short, char*, char*);								// Internal Debugger function, NORMALLY it is never called, only when FATAL error
																	// You NEED to provide this function, see examples!

MADLibrary* MADGetMADLibraryPtr();									// Get MADDriver structure pointer.

OSErr	MADInitLibrary( char *PlugsFolderName, Boolean sysMemory, MADLibrary **MADLib);	// Library initialisation, you have to CALL this function if you want to use other functions & variables

OSErr	MADDisposeLibrary( MADLibrary *MADLib);						// Close Library, close music, close driver, free all memory

void	MADGetBestDriver( MADDriverSettings	*DriverInitParam);		// Found and identify the current Mac sound hardware and fill DriverInitParam
OSErr	MADCreateDriver( MADDriverSettings	*DriverInitParam, MADLibrary *MADLib, MADDriverRec** returnDriver);		// Music Driver initialization and memory allocation
OSErr	MADDisposeDriver( MADDriverRec *MDriver);											// Dispose the music driver, use it after RInitMusic()

OSErr	MADChangeDriverSettings( MADDriverSettings	*DriverInitParam, MADDriverRec** returnDriver);

OSErr	MADStartDriver( MADDriverRec *MDriver);										// NEW - Activate the sound generating procedure (interruption)
OSErr	MADStopDriver( MADDriverRec *MDriver);										// NEW - DESActivate the sound generating procedure (interruption)

OSErr	MADPlayMusic( MADDriverRec *MDriver);										// NEW - Read and play current music in memory - Call MADStartInterruption BEFORE
OSErr	MADStopMusic( MADDriverRec *MDriver);										// NEW - Stop reading current music in memory, Use MADCleanDriver to stop sounds
void	MADCleanDriver( MADDriverRec *intDriver);									// Clean the driver : stop playing sounds

OSErr	MADReset( MADDriverRec *MDriver);											// Reset the current music at the start position
OSErr	MADGetMusicStatus( MADDriverRec *MDriver, long *fullTime, long *curTime);			// Get informations about music position and duration, IN 1/60th SECS !! NOT IN SECS ANYMORE !!!!!!!
OSErr	MADSetMusicStatus( MADDriverRec *MDriver, long minV, long maxV, long curV);			// Change position of current music, by example MADSetMusicStatus( 0, 100, 50) = go to the middle of the music

OSErr	MADAttachDriverToMusic( MADDriverRec *driver, MADMusic *music, unsigned char*);

OSErr	MADLoadMusicPtr( MADMusic **music, char *myPtr);								// MAD ONLY - Load a MAD Ptr into memory, you can free() your Ptr after this call

OSErr	MADLoadMusicFileCString( MADLibrary *, MADMusic **music, char *type, char *fName);			// Load a music file with plugs
#ifdef _MAC_H
OSErr	MADLoadMusicCFURLFile( MADLibrary *lib, MADMusic **music, OSType type, CFURLRef theRef);
#endif

OSErr	MADMusicIdentifyCString( MADLibrary *, char *type, Ptr cName);			// Identify what kind of music format is cName file.

#ifdef _MAC_H
OSErr	MADMusicIdentifyCFURL( MADLibrary *lib, OSType *type, CFURLRef URLRef); //Identify what kind of music format is URLRef file.
#endif


Boolean	MADPlugAvailable( MADLibrary *, char *type);								// Is plug 'type' available?

OSErr	MADDisposeMusic( MADMusic **, MADDriverRec *MDriver);								// Dispose the current music, use it after RLoadMusic(), RLoadMusicRsrc(), RInstallMADF()

void	MADChangeTracks( MADDriverRec *MDriver, short);				// Change current tracks number of the music driver
Cmd*	GetMADCommand(	short		position,						// Extract a Command from a PatData structure
						short		channel,
						PatData*	aPatData);

OSErr	MADPlaySoundData(	MADDriverRec *MDriver,
							char			*soundPtr,				// Sound Pointer to data
							size_t			size,					// Sound size in bytes
							SInt32			channel,				// channel ID on which to play sound
							SInt32			note,					// note: 0 to NUMBER_NOTES or 0xFF: play sound at 22 Khz
							SInt32			amplitude,				// 8 or 16 bits
							size_t			loopBeg,				// loop beginning
							size_t			loopSize,				// loop size in bytes
							double			rate,					// sample rate of the sound data, by ex: rate22khz
							Boolean			stereo);				// sample is in stereo or in mono?
							
#if 0
OSErr	MADPlaySoundDataSYNC(MADDriverRec *MDriver,
							char			*soundPtr,				// Sound Pointer to data
							long			size,					// Sound size in bytes
							long			channel,				// channel ID on which to play sound
							long			note,					// note: 0 to NUMBER_NOTES or 0xFF: play sound at 22 Khz
							long			amplitude,				// 8 or 16 bits
							long			loopBeg,				// loop beginning
							long			loopSize,				// loop size in bytes
							double			rate,					// sample rate of the sound data, by ex: rate22khz
							Boolean			stereo);				// sample is in stereo or in mono?
#endif

Boolean MADWasReading(MADDriverRec *driver);
void MADSetReading(MADDriverRec *driver, Boolean toSet);

#ifdef __cplusplus
}
#endif

#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
#pragma pack()
#endif
#endif
