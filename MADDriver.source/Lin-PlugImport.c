#include "RDriver.h"
#include "FileUtils.h"
#include "PPPrivate.h"
#include <dlfcn.h>

OSErr PPMADInfoFile(const char *AlienFile, PPInfoRec *InfoRec)
{
	MADSpec		*theMAD;
	long		fileSize;
	UNFILE		fileID;
	
	theMAD = (MADSpec*)malloc(sizeof(MADSpec) + 200);
	
	fileID = iFileOpenRead(AlienFile);
	if (!fileID) {
		free(theMAD);
		return -1;
	}
	fileSize = iGetEOF(fileID);
	
	iRead(sizeof(MADSpec), theMAD, fileID);
	iClose(fileID);
	
	strcpy(InfoRec->internalFileName, theMAD->name);
	
	InfoRec->totalPatterns = theMAD->numPat;
	InfoRec->partitionLength = theMAD->numPointers;
	InfoRec->totalTracks = theMAD->numChn;
	InfoRec->signature = 'MADK';
	strcpy( InfoRec->formatDescription, "MADK");
	InfoRec->totalInstruments = theMAD->numInstru;
	InfoRec->fileSize = fileSize;
	
	free(theMAD);
	theMAD = NULL;
	
	return noErr;
}

OSErr CallImportPlug(MADLibrary				*inMADDriver,
					 short					PlugNo,			// CODE du plug
					 OSType					order,
					 char					*AlienFile,
					 MADMusic				*theNewMAD,
					 PPInfoRec				*info)
{
	MADDriverSettings driverSettings = {0};
	
	return (*inMADDriver->ThePlug[PlugNo].IOPlug)(order, AlienFile, theNewMAD, info, driverSettings);
}

typedef OSErr (*FILLPLUG)(PlugInfo *);

void MInitImportPlug(MADLibrary *inMADDriver, Ptr PlugsFolderName)
{
	inMADDriver->ThePlug = (PlugInfo*) calloc( MAXPLUG, sizeof( PlugInfo));
	inMADDriver->TotalPlug = 0;
	//TODO: iterate plug-in paths
	int i =0;
	{
		inMADDriver->ThePlug[i].hLibrary = dlopen(NULL, RTLD_LAZY);
		FILLPLUG plugFill = (FILLPLUG)dlsym(inMADDriver->ThePlug[i].hLibrary, "FillPlug");
		inMADDriver->ThePlug[i].IOPlug = (MADPLUGFUNC)dlsym(inMADDriver->ThePlug[i].hLibrary, "PPImpExpMain");
		if(plugFill && inMADDriver->ThePlug[i].IOPlug)
		{
			(*plugFill)(&inMADDriver->ThePlug[i]);
			inMADDriver->TotalPlug++;
		}
	}
}

void CloseImportPlug(MADLibrary *inMADDriver)
{
	int i;
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		dlclose(inMADDriver->ThePlug[i].hLibrary);
	}
	free(inMADDriver->ThePlug);
}

OSErr PPInfoFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, PPInfoRec *InfoRec)
{
	short		i;
	MADMusic	aMAD;
	
	if (!strcmp(kindFile, "MADK")) {
		return PPMADInfoFile(AlienFile, InfoRec);
	}
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug( inMADDriver, i, MADPlugInfo, AlienFile, &aMAD, InfoRec);
		}
	}
	return MADCannotFindPlug;
}

OSErr PPImportFile( MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic **theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp( kindFile, inMADDriver->ThePlug[i].type)) {
			*theNewMAD = (MADMusic*) calloc(sizeof(MADMusic));
			if (!theNewMAD)
				return MADNeedMemory;
			
			return CallImportPlug(inMADDriver, i, MADPlugImport, AlienFile, *theNewMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

#define CharlMADcheckLength 10
OSErr CheckMADFile(const char* name)
{
	UNFILE				refNum;
	char				charl[CharlMADcheckLength];
	OSErr				err;
	
	refNum = iFileOpenRead(name);
	if (!refNum)
		return MADReadingErr;
	else {
		iRead(CharlMADcheckLength, charl, refNum);
		
		if (charl[0] == 'M' &&							// MADK
			charl[1] == 'A' &&
			charl[2] == 'D' &&
			charl[3] == 'K')
			err = noErr;
		else
			err = MADIncompatibleFile;
		
		iClose(refNum);
	}
	return err;
}

OSErr PPIdentifyFile(MADLibrary *inMADDriver, char *type, char *AlienFile)
{
	UNFILE				refNum;
	short				i;
	PPInfoRec			InfoRec;
	OSErr				iErr = noErr;
	
	strcpy( type, "!!!!");
	
	// Check if we have access to this file
	refNum = iFileOpenRead(AlienFile);
	if (!refNum)
		return MADReadingErr;
	else {
		if (iGetEOF(refNum) < 100)
			iErr = MADIncompatibleFile;
		iClose(refNum);
		if (iErr)
			return iErr;
	}
	
	// Is it a MAD file?
	iErr = CheckMADFile(AlienFile);
	if (iErr == noErr) {
		strcpy(type, "MADK");
		return noErr;
	}
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (CallImportPlug(inMADDriver, i, MADPlugTest, AlienFile, NULL, &InfoRec) == noErr) {
			strcpy(type, inMADDriver->ThePlug[i].type);
			return noErr;
		}
	}
	
	strcpy(type, "!!!!");
	return MADCannotFindPlug;
}

Boolean	MADPlugAvailable( MADLibrary *inMADDriver, char* kindFile)
{
	short		i;
	
	if (!strcmp(kindFile, "MADK"))
		return TRUE;
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return TRUE;
	}
	return FALSE;
}

OSErr PPExportFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile, MADMusic *theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug(inMADDriver, i, MADPlugExport, AlienFile, theNewMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

OSErr PPTestFile(MADLibrary *inMADDriver, char *kindFile, char *AlienFile)
{
	short			i;
	MADMusic		aMAD;
	PPInfoRec		InfoRec;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if (!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			return CallImportPlug( inMADDriver, i, MADPlugTest, AlienFile, &aMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

OSType GetPPPlugType(MADLibrary *inMADDriver, short ID, OSType mode)
{
	short i, x;
	
	if (ID >= inMADDriver->TotalPlug) PPDebugStr(__LINE__, __FILE__, "PP-Plug ERROR. ");
	
	for (i = 0, x = 0; i < inMADDriver->TotalPlug; i++) {
		if (inMADDriver->ThePlug[i].mode == mode || inMADDriver->ThePlug[i].mode == MADPlugImportExport) {
			if (ID == x) {
				short 	xx;
				OSType	type = '    ';
				
				xx = strlen(inMADDriver->ThePlug[i].type);
				if (xx > 4) xx = 4;
				memcpy(&type, inMADDriver->ThePlug[i].type, xx);
				PPBE32(&type);
				
				return type;
			}
			x++;
		}
	}
	
	PPDebugStr(__LINE__, __FILE__, "PP-Plug ERROR II.");
	
	return noErr;
}