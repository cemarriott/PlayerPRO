/*	v 1.0			*/
/*	1999 by ANR		*/

#include "PPPlug.h"
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>

OSErr main(		OSType					order,						// Order to execute
				InstrData				*InsHeader,					// Ptr on instrument header
				sData					**sample,					// Ptr on samples data
				short					*sampleID,					// If you need to replace/add only a sample, not replace the entire instrument (by example for 'AIFF' sound)
																	// If sampleID == -1 : add sample else replace selected sample.
				FSSpec					*AlienFileFSSpec,			// IN/OUT file
				PPInfoPlug				*thePPInfoPlug)
{
	OSErr	myErr = noErr;
	Ptr		AlienFile;
	UNFILE	iFileRefI;
	long	inOutBytes;
		
	switch (order)
	{
		case 'PLAY':
		break;
		
		case 'IMPL':
		{
			Ptr				theSound;
			long			lS, lE;
			short			sS;
			unsigned long	rate;
			Boolean			stereo;
			FSSpec			newFile;
			
			myErr = ConvertDataToWAVE(*AlienFileFSSpec, &newFile, thePPInfoPlug);
			if (myErr == noErr)
			{
				theSound = ConvertWAV(&newFile, &lS, &lE, &sS, &rate, &stereo);
				
				if (theSound) inAddSoundToMAD(theSound, lS, lE, sS, 60, rate, stereo, newFile.name, InsHeader, sample, sampleID);
				else myErr = MADNeedMemory;
				
				FSpDelete(&newFile);
			}
		}
		break;
		
		case 'TEST':
		{
			FInfo fInfo;
			
			FSpGetFInfo(AlienFileFSSpec, &fInfo);
			
			if (fInfo.fdType == 'MPEG') myErr = noErr;
			else myErr = MADFileNotSupportedByThisPlug;
		}
		break;
		
		case 'EXPL':
		break;
		
		default:
			myErr = MADOrderNotImplemented;
		break;
	}
		
	return myErr;
}
