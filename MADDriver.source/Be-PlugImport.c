/********************						***********************/
//
//	Player PRO 5.0 -- MAD Class for BeOS -
//
//	Library Version 5.0
//
//	To use with MAD Library for BeOS: CodeWarrior
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	Thank you for your interest in PlayerPRO !
//
//	Special Thanks to:
//
//	Dario Accornero <adario@cs.bu.edu>
//
//	For his BeOS support and help!
//
//	FAX:			(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 		rossetantoine@bluewin.ch
//
/********************						***********************/

#include "RDriver.h"
#include "RDriverInt.h"
#include "FileUtils.h"

static Boolean LoadPlugLib(const char *name, PlugInfo* plug);
static void ScanPlugDir(const char *name);

//static MADLibrary *inMADDriver;

OSErr CheckMADFile(const char *name)
{
	UNFILE	refNum;
	char	charl[20];
	OSErr	err;
	
	refNum = iFileOpenRead(name);
	if(!refNum)
		return MADReadingErr;
	else {
		iRead(10, charl, refNum);
		
		if(charl[0] == 'M' &&
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

//	DA on 9/9/98

OSErr CallImportPlug(short		PlugNo,
					 OSType		order,
					 char		*AlienFile,
					 MADMusic	*theNewMAD,
					 PPInfoRec	*info)
{
	OSErr				myErr;
	MADDriverSettings	settings = {0};
	
	myErr = (*(inMADDriver->ThePlug[PlugNo].IOPlug))(order, AlienFile, theNewMAD, info, &settings);
	
	return myErr;
}

OSErr PPTestFile(char *kindFile, char *AlienFile)
{
	short			i;
	MADMusic		aMAD;
	PPInfoRec		InfoRec;
	
	for(i = 0; i < inMADDriver->TotalPlug; i++) {
		if(!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return (CallImportPlug(i, MADPlugTest, AlienFile, &aMAD, &InfoRec));
	}
	return MADCannotFindPlug;
}

OSErr PPMADInfoFile(char *AlienFile, PPInfoRec *InfoRec)
{
	MADSpec		*theMAD;
	long		fileSize;
	UNFILE		fileID;
	
	theMAD = (MADSpec*)malloc(sizeof(MADSpec) + 200);
	
	fileID = iFileOpenRead(AlienFile);
	if(!fileID) {
		free(theMAD);
		return MADReadingErr;
	}
	fileSize = iGetEOF(fileID);
	
	iRead(sizeof(MADSpec), theMAD, fileID);
	iClose(fileID);
	
	strcpy(InfoRec->internalFileName, theMAD->name);
	
	InfoRec->totalPatterns = theMAD->numPat;
	InfoRec->partitionLength = theMAD->numPointers;
	InfoRec->totalTracks = theMAD->numChn;
	InfoRec->signature = 'MADK';
	strcpy(InfoRec->formatDescription, "MADK");
	InfoRec->totalInstruments = theMAD->numInstru;
	InfoRec->fileSize = fileSize;
	
	free(theMAD);
	theMAD = NULL;
	
	return noErr;
}

OSErr PPInfoFile(char *kindFile, char *AlienFile, PPInfoRec *InfoRec)
{
	short		i;
	MADMusic	aMAD;
	
	if(!strcmp(kindFile, "MADK"))
		return PPMADInfoFile(AlienFile, InfoRec);

	for(i = 0; i < inMADDriver->TotalPlug; i++) {
		if(!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return CallImportPlug(i, MADPlugInfo, AlienFile, &aMAD, InfoRec);
	}
	return MADCannotFindPlug;
}

OSErr PPExportFile(char *kindFile, char *AlienFile, MADMusic *theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for(i = 0; i < inMADDriver->TotalPlug; i++) {
		if(!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return CallImportPlug(i, MADPlugExport, AlienFile, theNewMAD, &InfoRec);
	}
	return MADCannotFindPlug;
}

OSErr PPImportFile(MADLibrary *MADLib, char *kindFile, char *AlienFile, MADMusic **theNewMAD)
{
	short		i;
	PPInfoRec	InfoRec;
	
	for(i = 0; i < inMADDriver->TotalPlug; i++) {
		if(!strcmp(kindFile, inMADDriver->ThePlug[i].type)) {
			*theNewMAD = (MADMusic*)calloc(sizeof(MADMusic), 1);
			if (!*theNewMAD)
				return MADNeedsMemory;
			//TODO: clean-up if there's a failure.
			return CallImportPlug(i, MADPlugImport, AlienFile, *theNewMAD, &InfoRec);
		}
	}
	return MADCannotFindPlug;
}

OSErr PPIdentifyFile(MADLibrary *lib, char *type, char *AlienFile)
{
	FILE*		refNum;
	short		i;
	PPInfoRec	InfoRec;
	OSErr		iErr;
	
	strcpy(type, "!!!!");
	
	// Check if we have access to this file
	refNum = iFileOpenRead(AlienFile);
	if(!refNum)
		return MADReadingErr;
	iClose(refNum);
	
	// Is it a MAD file?
	iErr = CheckMADFile(AlienFile);
	if(iErr == noErr) {
		strcpy(type, "MADK");
		return noErr;
	}
	
	for(i = 0; i < inMADDriver->TotalPlug; i++)
	{
		if(CallImportPlug(i, MADPlugTest, AlienFile, 0L, &InfoRec) == noErr) {
			strcpy(type, inMADDriver->ThePlug[i].type);
			
			return noErr;
		}
	}
	
	strcpy(type, "!!!!");
	
	return MADCannotFindPlug;
}

Boolean	MADPlugAvailable(char *kindFile)
{
	short		i;

	if(!strcmp(kindFile, "MADK"))
		return true;
	
	for (i = 0; i < inMADDriver->TotalPlug; i++) {
		if(!strcmp(kindFile, inMADDriver->ThePlug[i].type))
			return true;
	}
	return false;
}

//	DA on 9/9/98

static Boolean LoadPlugLib(const char *name, PlugInfo* plug)
{
	strcpy(plug->file, name);
	
	plug->hLibrary = load_add_on(name);
	if (!plug->hLibrary)
		return false;
	
	OSErr (*FillPlugCode)(PlugInfo *p);
	FillPlugCode = NULL;
	status_t symbolGet = get_image_symbol(plug->hLibrary, "FillPlug", B_SYMBOL_TYPE_TEXT, &FillPlugCode );
	
	if (symbolGet == B_NO_ERROR && FillPlugCode) {
		FillPlugCode(plug);
	
		MADPlug	plugCode = NULL;
		
		symbolGet = get_image_symbol(plug->hLibrary, "PPImpExpMain", B_SYMBOL_TYPE_TEXT, &plugCode);
		if (symbolGet == B_NO_ERROR && plugCode) {
			plug->IOPlug = plugCode;
			return true;
		} else {
			unload_add_on(plug->hLibrary);
			return	false;
		}
	} else {
		unload_add_on(plug->hLibrary);
		return false;
	}
}

//	DA on 9/9/98

static void ScanPlugDir(const char *FindFolder)
{
	BDirectory	plugsDir(FindFolder); //Check this! This looks like C++ code
	entry_ref	plugsRef;
	while(plugsDir.GetNextRef(&plugsRef) == B_NO_ERROR) {
		if(inMADDriver->TotalPlug < MAXPLUG) {
			char myCompleteFilename[1024];
			sprintf(myCompleteFilename, "%s/%s", FindFolder, plugsRef.name);
			if (LoadPlugLib(myCompleteFilename, &inMADDriver->ThePlug[inMADDriver->TotalPlug]))
				inMADDriver->TotalPlug++;
		}
	}
}

//	DA on 9/9/98

void MInitImportPlug(MADLibrary *lib, const char *PlugsFolderName)
{
	//	Signal there are no valid plugs yet.

	inMADDriver = lib;
	inMADDriver->TotalPlug = 0;
	inMADDriver->ThePlug = (PlugInfo*)calloc(MAXPLUG, sizeof(PlugInfo));

	//	Construct valid path, independent of launch mode.
	
	app_info	aInfo;
	status_t	error = be_app->GetAppInfo(&aInfo);
	BEntry		fileEntry(&aInfo.ref);
	BPath		filePath;
	fileEntry.GetPath(&filePath);
	BPath		parentPath;
	filePath.GetParent(&parentPath);
	char		pathBuffer[1024];
	strcpy(pathBuffer, parentPath.Path()); //Check this! This looks like C++ code

	//	Build main plugs path and scan it.

	char		FindFolder[1024];	
	if (PlugsFolderName && strlen(PlugsFolderName))
		sprintf(FindFolder, "%s/%s", pathBuffer, PlugsFolderName);
	else
		strcpy(FindFolder, pathBuffer);
	
	ScanPlugDir(FindFolder);
}

//	DA on 9/9/98

void CloseImportPlug(MADLibrary *lib)
{
	short i;
	
	for (i = 0; i < lib->TotalPlug; i++)
		unload_add_on(lib->ThePlug[i].hLibrary);
	
	free(lib->ThePlug);
	lib->ThePlug = NULL;
}