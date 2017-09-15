//
//  PPSmoothPlug.m
//  PPMacho
//
//  Created by C.W. Betts on 9/7/14.
//
//

#import "PPSmoothPlug.h"
#import <PlayerPROKit/PPSampleObject.h>
//	Usage:
//	Works like a low pass filter, removing high
//	harmonics generated by a low sampling rate.
//	Works on the selected part or all the waveform
//	if there is no selection.
//	Not very accurate, but pretty fast to write !!!

@implementation PPSmoothPlug

- (BOOL)hasUIConfiguration
{
	return NO;
}

- (instancetype)initForPlugIn
{
	return self = [self init];
}

- (BOOL)runWithData:(inout PPSampleObject *)theData selectionRange:(NSRange)selRange onlyCurrentChannel:(BOOL)StereoMode driver:(PPDriver *)driver error:(NSError * _Nullable __autoreleasing * _Nonnull)error
{
	long i, length, temp, prevtemp, nexttemp, work;
	length = selRange.length - 1;
	
	NSMutableData *ourData = [theData.data mutableCopy];
	char *ourModData8 = ourData.mutableBytes;
	
	switch (theData.amplitude) {
		case 8:
		{
			Ptr	SamplePtr = ourModData8 + selRange.location;
			
			prevtemp = *SamplePtr++;
			temp = *SamplePtr++;
			for (i = 1; i < length; i++) {
				nexttemp = *SamplePtr--;
				
				work = ((prevtemp + nexttemp) + (temp * 6)) >> 3;
				
				*SamplePtr++ = work;
				prevtemp = temp;
				temp = nexttemp;
				SamplePtr++;
			}
		}
			break;
			
		case 16:
		{
			short	*SamplePtr = (short*)ourModData8 + (selRange.location / 2);
			
			prevtemp = *SamplePtr++;
			temp = *SamplePtr++;
			for (i = 1; i < length / 2; i++) {
				nexttemp = *SamplePtr--;
				
				work = ((prevtemp + nexttemp) + (temp * 6)) >> 3;
				
				*SamplePtr++ = work;
				prevtemp = temp;
				temp = nexttemp;
				SamplePtr++;
			}
		}
			break;
	}
	
	theData.data = ourData;
	
	return YES;
}

@end
