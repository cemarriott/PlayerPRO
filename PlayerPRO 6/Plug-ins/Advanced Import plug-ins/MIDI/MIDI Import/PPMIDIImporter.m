//
//  PPMIDIImportHelper.m
//  PPMacho
//
//  Created by C.W. Betts on 8/6/14.
//
//

#import "PPMIDIImporter.h"

#include <PlayerPROCore/PlayerPROCore.h>
#include <string.h>
#include "PTMID.h"

#ifdef __LP64__
#error This code will only work on 32-bit mode.
#error This code needs to be rewritten.
#endif

/**************************************************************************
 **************************************************************************/

static unsigned char* MYC2PStr(Ptr cStr)
{
	long length = strlen(cStr);
	memmove(cStr + 1, cStr, length);
	cStr[0] = length;
	return (unsigned char*)cStr;
}

static MADErr TestMIDIFile(const void *AlienFile)
{
	if (memcmp(AlienFile, "MThd", 4) == 0)
		return MADNoErr;
	else
		return MADFileNotSupportedByThisPlug;
}

void CreateResult(Ptr aPtr)
{
	MYC2PStr(aPtr);
	DebugStr((unsigned char *) aPtr);
}

void ConvertMidiFile(const char *src, MADMusic *theMAD, MADDriverSettings *init);

@implementation PPMIDIImporter

+ (instancetype)sharedImporter
{
	static dispatch_once_t onceToken;
	static PPMIDIImporter *shared;
	dispatch_once(&onceToken, ^{
		shared = [PPMIDIImporter new];
	});
	return shared;
}

+ (MADErr)fillData:(inout NSMutableData*)theData withMusic:(in MADMusic*)music
{
	int		alpha = 0;
	int		i, x;
	size_t	inOutCount;
	MADErr	theErr = MADNoErr;
	MADSpec aHeader;
	
	if (music->musicUnderModification)
		return MADWritingErr;
	
	//TODO: error-checking
	
	music->musicUnderModification = TRUE;
	for (i = 0, x = 0; i < MAXINSTRU; i++) {
		music->fid[i].no = i;
		
		// Is there something in this instrument?
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {
			x++;
		}
	}
	
	music->header->numInstru = x;
	
	aHeader = *music->header;
	[theData appendBytes:&aHeader length:sizeof(MADSpec)];
	
	for (i = 0; i < music->header->numPat; i++) {
		if (music->partition[i]) {
			PatData *tmpPat;
			inOutCount = sizeof(PatHeader);
			inOutCount += music->header->numChn * music->partition[i]->header.size * sizeof(Cmd);
			tmpPat = calloc(inOutCount, 1);
			memcpy(tmpPat, music->partition[i], inOutCount);
			tmpPat->header.compMode = 'NONE';
			[theData appendBytes:tmpPat length:inOutCount];
			free(tmpPat);
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		if (music->fid[i].numSamples > 0 || music->fid[i].name[0] != 0) {	// Is there something in this instrument?
			InstrData instData;
			music->fid[i].no = i;
			instData = music->fid[i];
			[theData appendBytes:&instData length:sizeof(InstrData)];
		}
	}
	
	for (i = 0; i < MAXINSTRU; i++) {
		for (x = 0; x < music->fid[i].numSamples; x++) {
			sData	curData;
			sData32	copyData;
			void	*dataCopy = NULL;
			curData = *music->sample[music->fid[i].firstSample + x];
			
			inOutCount = sizeof(sData32);
			memcpy(&copyData, &curData, inOutCount);
			copyData.data = 0;
			[theData appendBytes:&copyData length:inOutCount];
			
			inOutCount = music->sample[music->fid[i].firstSample + x]->size;
			dataCopy = malloc(inOutCount);
			memcpy(dataCopy, curData.data, inOutCount);
			[theData appendBytes:dataCopy length:inOutCount];
			free(dataCopy);
		}
	}
	
	// EFFECTS *** *** *** *** *** *** *** *** *** *** *** ***
	
	for (i = 0; i < 10 ; i++) {	// Global Effects
		if (music->header->globalEffect[i]) {
			FXSets aSet;
			inOutCount = sizeof(FXSets);
			aSet = music->sets[alpha];
			[theData appendBytes:&aSet length:inOutCount];
			alpha++;
		}
	}
	
	for (i = 0; i < music->header->numChn; i++) {	// Channel Effects
		for (x = 0; x < 4; x++) {
			if (music->header->chanEffect[i][x]) {
				FXSets aSet;
				inOutCount = sizeof(FXSets);
				aSet = music->sets[alpha];
				[theData appendBytes:&aSet length:inOutCount];
				alpha++;
			}
		}
	}
	
	music->header->numInstru = MAXINSTRU;
	
	return theErr;
}

- (void)importMIDIFileAtURL:(NSURL*)theURL numberOfTracks:(NSInteger)trackNum useQTInstruments:(BOOL)qtIns withReply:(void (^)(NSData *theData, MADErr error))reply
{
	MADErr theErr = MADNoErr;
	UseQKIns = qtIns;
	trackNum /= 2;
	trackNum *= 2;
	wMaxchan = trackNum;
	MADDriverSettings init = {0};
	MADMusic *MadFile = malloc(sizeof(MADMusic) + 20);
	NSMutableData *madData = [[NSMutableData alloc] initWithCapacity:128];
	NSData *fileData = [[NSData alloc] initWithContentsOfURL:theURL];
	if (!fileData) {
		[madData release];
		free(MadFile);
		reply(nil, MADReadingErr);
		return;
	}
	const void *AlienFile = [fileData bytes];
	theErr = TestMIDIFile(AlienFile);
	if (theErr != MADNoErr) {
		[fileData release];
		[madData release];
		free(MadFile);
		reply(nil, theErr);
		return;
	}
	ConvertMidiFile(AlienFile, MadFile, &init);
	[fileData release];
	
	theErr = [PPMIDIImporter fillData:madData withMusic:MadFile];
	MADDisposeMusic(&MadFile, NULL);
	
	reply(madData, theErr);
	[madData autorelease];
}

- (BOOL)listener:(NSXPCListener *)listener shouldAcceptNewConnection:(NSXPCConnection *)newConnection
{
	newConnection.exportedInterface = [NSXPCInterface interfaceWithProtocol:@protocol(PPMIDIImportHelper)];
	newConnection.exportedObject = self;
	[newConnection resume];
	
	return YES;
}

@end
