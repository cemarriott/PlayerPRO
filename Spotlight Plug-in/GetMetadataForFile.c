#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include "RDriver.h"
#include "FileUtils.h"
#include "GetMetadataForFile.h"

/* -----------------------------------------------------------------------------
   Step 1
   Set the UTI types the importer supports
  
   Modify the CFBundleDocumentTypes entry in Info.plist to contain
   an array of Uniform Type Identifiers (UTI) for the LSItemContentTypes 
   that your importer can handle
  
   ----------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
   Step 2 
   Implement the GetMetadataForFile function
  
   Implement the GetMetadataForFile function below to scrape the relevant
   metadata from your document and return it as a CFDictionary using standard keys
   (defined in MDItem.h) whenever possible.
   ----------------------------------------------------------------------------- */

/* -----------------------------------------------------------------------------
   Step 3 (optional) 
   If you have defined new attributes, update the schema.xml file
  
   Edit the schema.xml file to include the metadata keys that your importer returns.
   Add them to the <allattrs> and <displayattrs> elements.
  
   Add any custom types that your importer requires to the <attributes> element
  
   <attribute name="com_mycompany_metadatakey" type="CFString" multivalued="true"/>
  
   ----------------------------------------------------------------------------- */

const CFStringRef kPPMDInstumentsList = CFSTR("net_sourceforge_playerpro_tracker_instumentlist");
const CFStringRef kPPMDPatternList = CFSTR("net_sourceforge_playerpro_tracker_patternlist");
const CFStringRef kPPMDTotalPatterns = CFSTR("net_sourceforge_playerpro_tracker_totalpatterns");
const CFStringRef kPPMDPartitionLength = CFSTR("net_sourceforge_playerpro_tracker_partitionlength");
const CFStringRef kPPMDTotalInstruments = CFSTR("net_sourceforge_playerpro_tracker_totalinstruments");
const CFStringRef kPPMDTotalTracks = CFSTR("net_sourceforge_playerpro_tracker_totaltracks");
const CFStringRef kPPMDFormatDescription = CFSTR("net_sourceforge_playerpro_tracker_formatdescription");

/* -----------------------------------------------------------------------------
    Get metadata attributes from file
   
   This function's job is to extract useful information your file format supports
   and return it as a dictionary
   ----------------------------------------------------------------------------- */

Boolean GetMetadataForFile(void* thisInterface,
			   CFMutableDictionaryRef attributes, 
			   CFStringRef contentTypeUTI,
			   CFStringRef pathToFile)
{
	//Before we do anything else, check to make sure it's not the Windows file winoldap.mod
	//This file seems to crash the metadata importer, even though the proper plug-in should
	//Say that it can't open it.
	{
		CFURLRef theRef = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, pathToFile, kCFURLPOSIXPathStyle, FALSE);
		CFStringRef lastPathName = CFURLCopyLastPathComponent(theRef);
		CFRelease(theRef);
		CFComparisonResult result = CFStringCompare(lastPathName, CFSTR("winoldap.mod"), kCFCompareCaseInsensitive | kCFCompareWidthInsensitive);
		CFRelease(lastPathName);
		if (result == kCFCompareEqualTo) return FALSE;
	}
	
	/* Pull any available metadata from the file at the specified path */
	/* Return the attribute keys and attribute values in the dict */
	/* Return TRUE if successful, FALSE if there was no data provided */
#ifdef IMPORT_COCOA_BUNDLE
	if (UTTypeConformsTo(contentTypeUTI, CFSTR("net.sourceforge.playerpro.mad-bundle"))) {
		return GetMetadataForPackage(attributes, pathToFile);
	}
#endif
	MADDriverRec			*MADDriver;
	MADMusic				*MADMusic1;
	MADLibrary				*MADLib;
	MADDriverSettings		init;
	
	MADGetBestDriver(&init);
	init.driverMode = NoHardwareDriver;
	
	if (MADInitLibrary(NULL, &MADLib) != noErr) return FALSE;
	if (MADCreateDriver(&init, MADLib, &MADDriver) != noErr)
	{
		MADDisposeLibrary(MADLib);
		return FALSE;
	}
	
	{
		char		type[5];
		char		utiType[5] = {0};
		{
			OSType info;
			CFStringRef ostypes;
			//Try to get the OSType of the UTI.
			ostypes = UTTypeCopyPreferredTagWithClass(contentTypeUTI, kUTTagClassOSType);
			
			info = UTGetOSTypeFromString(ostypes);
			if (ostypes) {
				CFRelease(ostypes);
			}
			if (info) {
				OSType2Ptr(info, utiType);
			} else {
				strcpy(utiType, "!!!!");
			}
		}
		
		char *path = NULL;
		{
			char *fullPath = NULL;
			CFIndex maxLen = CFStringGetMaximumSizeOfFileSystemRepresentation(pathToFile);
			fullPath = malloc(maxLen);
			if (CFStringGetFileSystemRepresentation(pathToFile, fullPath, maxLen) == FALSE)
			{
				free(fullPath);
				MADDisposeDriver(MADDriver);
				MADDisposeLibrary(MADLib);
				return FALSE;
			}
			size_t shortLen = strlen(fullPath);
			path = malloc(++shortLen);
			strlcpy(path, fullPath, shortLen);
			free(fullPath);
		}
		
		
		if(MADMusicIdentifyCString(MADLib, type, path) != noErr)
		{
			//Couldn't identify via raw file, try by UTI
			//CFRelease(tempRef);
			//goto fail1;
			strcpy(type, utiType);
		}
		
#ifdef DEBUG
		if (strcmp(utiType, "!!!!") == 0) {
			printf("PPImporter: Unable to determine file type based on UTI.\n");
		}else if (strcmp(utiType, type) != 0) {
			printf("PPImporter: File types differ, UTI says %s, PlayerPRO says %s.\n", utiType, type);
		}
#endif
		
		if( MADPlugAvailable( MADLib, type))		// Is available a plug to open this file?
		{
			OSErr err = noErr;
			err = MADLoadMusicFileCString(MADLib, &MADMusic1, type, path);
			if(err != noErr)
			{
				free(path);
				goto fail1;
			}
		} else {
			free(path);
			goto fail1;
		}
		
		{
			PPInfoRec rec;
			{
				char sig[5];
				if(MADMusicInfoCString(MADLib, type, path, &rec) != noErr) goto skipInfo;
				OSType2Ptr(rec.signature, sig);
				CFStringRef CFSig = CFStringCreateWithCString(kCFAllocatorDefault, sig, kCFStringEncodingMacRoman);
				if (!CFSig) {
					CFSig = CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("%08x"), (unsigned int)rec.signature);
				}
				CFMutableArrayRef codecArray = CFArrayCreateMutable(kCFAllocatorDefault, 1, &kCFTypeArrayCallBacks);
				CFArrayAppendValue(codecArray, CFSig);
				CFRelease(CFSig);
				
				CFDictionarySetValue(attributes, kMDItemCodecs, codecArray);
				CFRelease(codecArray);
			}
			//Set the title metadata
			CFStringRef title = CFStringCreateWithCString(kCFAllocatorDefault, rec.internalFileName, kCFStringEncodingMacRoman); //TODO: Check for other encodings?
			CFDictionarySetValue(attributes, kMDItemTitle, title);
			CFRelease(title);
			CFNumberRef tempNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &rec.totalPatterns);
			CFDictionarySetValue(attributes, kPPMDTotalPatterns, tempNum);
			CFRelease(tempNum); tempNum = NULL;
			tempNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &rec.partitionLength);
			CFDictionarySetValue(attributes, kPPMDPartitionLength, tempNum);
			CFRelease(tempNum); tempNum = NULL;
			tempNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &rec.totalInstruments);
			CFDictionarySetValue(attributes, kPPMDTotalInstruments, tempNum);
			CFRelease(tempNum); tempNum = NULL;
			tempNum = CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &rec.totalTracks);
			CFDictionarySetValue(attributes, kPPMDTotalTracks, tempNum);
			CFRelease(tempNum);
			{
				CFStringRef FormatDes = CFStringCreateWithCString(kCFAllocatorDefault, rec.formatDescription, kCFStringEncodingMacRoman);
				CFDictionarySetValue(attributes, kPPMDFormatDescription, FormatDes);
				CFRelease(FormatDes);
			}
		}
	skipInfo:
		free(path);
	}
	
	{
		//Set duration metadata
		MADAttachDriverToMusic( MADDriver, MADMusic1, NULL);
		long fT, cT;
		MADGetMusicStatus( MADDriver, &fT, &cT);	// Some infos about current music
		double fTd = fT / 60.0;
		
		CFNumberRef duration = CFNumberCreate(kCFAllocatorDefault, kCFNumberDoubleType, &fTd);
		CFDictionarySetValue(attributes, kMDItemDurationSeconds, duration);
		CFRelease(duration);
	}
	
	{
		CFMutableArrayRef InstruArray = CFArrayCreateMutable(kCFAllocatorDefault, MAXINSTRU * MAXSAMPLE, &kCFTypeArrayCallBacks);
		int	i;
		
		for( i = 0; i < MAXINSTRU ; i++)
		{
			InstrData *tempData = &MADMusic1->fid[i];
			
			CFStringRef temp = CFStringCreateWithCString(kCFAllocatorDefault, tempData->name, kCFStringEncodingMacRoman);//TODO: check for other encodings?
			if (!(CFEqual(CFSTR(""), temp))) {
				CFArrayAppendValue(InstruArray, temp);
			}
			CFRelease(temp);
			int sDataCount = tempData->firstSample + tempData->numSamples;
			int x;
			for (x = tempData->firstSample; x < sDataCount; x++) {
				sData *tempSData = MADMusic1->sample[x];
				temp = CFStringCreateWithCString(kCFAllocatorDefault, tempSData->name, kCFStringEncodingMacRoman);
				if (!CFEqual(CFSTR(""), temp)) {
					CFArrayAppendValue(InstruArray, temp);
				}
				CFRelease(temp);
			}
		}
		
		CFDictionarySetValue(attributes, kPPMDInstumentsList, InstruArray);
		CFRelease(InstruArray);
	}
	
	{
		CFMutableArrayRef PatArray = CFArrayCreateMutable(kCFAllocatorDefault, MAXPATTERN, &kCFTypeArrayCallBacks);
		int i;
		for( i = 0; i < MAXPATTERN; i++)
		{
			if (MADMusic1->partition != NULL && MADMusic1->partition[i] != NULL)
			{
				CFStringRef temp = CFStringCreateWithCString(kCFAllocatorDefault, MADMusic1->partition[i]->header.name, kCFStringEncodingMacRoman);//TODO: check for other encodings?
				if (!(CFEqual(CFSTR(""), temp))) {
					CFArrayAppendValue(PatArray, temp);
				}
				CFRelease(temp);
			}
		}
		CFDictionarySetValue(attributes, kPPMDPatternList, PatArray);
		CFRelease(PatArray);
	}
	
	MADCleanDriver( MADDriver);
	MADDisposeMusic( &MADMusic1, MADDriver);	// Dispose the current music
	MADStopDriver(MADDriver);					// Stop driver interrupt function
	MADDisposeDriver(MADDriver);				// Dispose music driver
	MADDisposeLibrary(MADLib);					// Close music library
	return TRUE;
	
fail1:
	MADStopDriver(MADDriver);				// Stop driver interrupt function
	MADDisposeDriver(MADDriver);			// Dispose music driver
	MADDisposeLibrary(MADLib);				// Close music library
	
	return FALSE;
}
