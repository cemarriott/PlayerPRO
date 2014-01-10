//
//  PPFXSetObject.m
//  PPMacho
//
//  Created by C.W. Betts on 1/8/14.
//
//

#import "PPFXSetObject.h"

#define kPPTrack @"PlayerPROKit FXSets Track"
#define kPPIdentifier @"PlayerPROKit FXSets Identifier"
#define kPPFXIdentifier @"PlayerPROKit FXSets FXIdentifier"
#define kPPArgumentNumbers @"PlayerPROKit FXSets Argument Numbers"
#define kPPValues @"PlayerPROKit FXSets Values"
#define kPPName @"PlayerPROKit FXSets Name"

@interface PPFXSetObject ()
@property (readwrite, strong) NSMutableArray *sets;
@end

@implementation PPFXSetObject
@synthesize theSet;

- (id)initWithFXSet:(FXSets*)theSett
{
	if (self = [super init]) {
		if (!theSett) {
			return nil;
		}
		theSet = *theSett;
		_sets = [[NSMutableArray alloc] initWithCapacity:100];
		for (int i = 0; i < 100; i++) {
			[self.sets addObject:@(theSet.values[i])];
		}
		_name = CFBridgingRelease(CFStringCreateWithPascalString(kCFAllocatorDefault, theSet.name, kCFStringEncodingMacRoman));
	}
	return self;
}

- (short)track
{
	return theSet.track;
}

- (void)setTrack:(short)track
{
	[self willChangeValueForKey:@"track"];
	theSet.track = track;
	[self didChangeValueForKey:@"track"];
}

- (short)identifier
{
	return theSet.id;
}

- (void)setIdentifier:(short)identifier
{
	[self willChangeValueForKey:@"identifier"];
	theSet.id = identifier;
	[self didChangeValueForKey:@"identifier"];
}

- (int)fxIdentifier
{
	return theSet.FXID;
}

- (void)setFxIdentifier:(int)fxIdentifier
{
	[self willChangeValueForKey:@"fxIdentifier"];
	theSet.FXID = fxIdentifier;
	[self didChangeValueForKey:@"fxIdentifier"];
}

- (short)argumentNumbers
{
	return theSet.noArg;
}

- (void)setArgumentNumbers:(short)argumentNumbers
{
	[self willChangeValueForKey:@"argumentNumbers"];
	theSet.noArg = argumentNumbers;
	[self didChangeValueForKey:@"argumentNumbers"];
}

- (NSArray*)values
{
	return [NSArray arrayWithArray:_sets];
}

- (FXSets)theSet
{
	[self writeBackToStruct];
	return theSet;
}

- (void)replaceSetAtIndex:(NSInteger)theLoc withFloat:(float)theNum
{
	[self replaceSetAtIndex:theLoc withNumber:@(theNum)];
}

- (void)replaceSetAtIndex:(NSInteger)theLoc withDouble:(double)theNum
{
	[self replaceSetAtIndex:theLoc withNumber:@(theNum)];
}

- (void)replaceSetAtIndex:(NSInteger)theLoc withInt:(int)theNum
{
	[self replaceSetAtIndex:theLoc withNumber:@(theNum)];
}

- (void)replaceSetAtIndex:(NSInteger)theLoc withInteger:(NSInteger)theNum
{
	[self replaceSetAtIndex:theLoc withNumber:@(theNum)];
}

- (void)replaceSetAtIndex:(NSInteger)theLoc withNumber:(NSNumber*)theNum
{
	NSParameterAssert(theLoc < 100 && theLoc > -1);
	NSIndexSet *tmpIdx = [NSIndexSet indexSetWithIndex:theLoc];
	[self willChange:NSKeyValueChangeReplacement valuesAtIndexes:tmpIdx forKey:@"sets"];
	[self willChange:NSKeyValueChangeReplacement valuesAtIndexes:tmpIdx forKey:@"values"];
	self.sets[theLoc] = theNum;
	[self didChange:NSKeyValueChangeReplacement valuesAtIndexes:tmpIdx forKey:@"sets"];
	[self didChange:NSKeyValueChangeReplacement valuesAtIndexes:tmpIdx forKey:@"values"];
}

- (void)writeBackToStruct
{
	//We only need to worry about the name and sets
	dispatch_apply(100, dispatch_get_global_queue(0, 0), ^(size_t i) {
		theSet.values[i] = [_sets[i] floatValue];
	});
	CFStringGetPascalString((__bridge CFStringRef)_name, theSet.name, 63, kCFStringEncodingMacRoman);
}

#pragma mark NSCopying protocol

- (id)copyWithZone:(NSZone *)zone
{
	PPFXSetObject *new = [[[self class] alloc] initWithFXSet:&theSet];
	if (new) {
		new.sets = [_sets mutableCopy];
		new.name = _name;
	}
	
	return new;
}

#pragma mark NSCoding protocol

- (id)initWithCoder:(NSCoder *)aDecoder
{
	if (self = [super init]) {
		self.name = [aDecoder decodeObjectForKey:kPPName];
		self.fxIdentifier = [aDecoder decodeIntForKey:kPPFXIdentifier];
		self.sets = [(NSArray*)[aDecoder decodeObjectForKey:kPPValues] mutableCopy];
		self.track = [(NSNumber*)[aDecoder decodeObjectForKey:kPPTrack] shortValue];
		self.identifier = [(NSNumber*)[aDecoder decodeObjectForKey:kPPIdentifier] shortValue];
		self.argumentNumbers = [(NSNumber*)[aDecoder decodeObjectForKey:kPPArgumentNumbers] shortValue];
		[self writeBackToStruct];
	}
	return self;
}

- (void)encodeWithCoder:(NSCoder *)aCoder
{
	[aCoder encodeObject:_name forKey:kPPName];
	[aCoder encodeObject:_sets forKey:kPPValues];
	[aCoder encodeInt:theSet.FXID forKey:kPPFXIdentifier];
	[aCoder encodeObject:@(theSet.track) forKey:kPPTrack];
	[aCoder encodeObject:@(theSet.noArg) forKey:kPPArgumentNumbers];
	[aCoder encodeObject:@(theSet.id) forKey:kPPIdentifier];
}

@end