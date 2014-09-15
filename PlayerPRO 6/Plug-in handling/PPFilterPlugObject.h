//
//  PPFilterPlugObject.h
//  PPMacho
//
//  Created by C.W. Betts on 1/22/13.
//
//

#import <Foundation/Foundation.h>
#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/MADPlug.h>
#import "PPPlugInObject.h"

@interface PPFilterPlugObject : PPPlugInObject
- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (instancetype)initWithBundle:(NSBundle *)aBund NS_DESIGNATED_INITIALIZER;

- (MADErr)callPluginWithData:(sData *)theData selectionStart:(long) SelectionStart selectionEnd:(long) SelectionEnd plugInInfo:(PPInfoPlug *)thePPInfoPlug stereoMode:(short)stereoMode;
@end
