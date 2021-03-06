//
//  AmplitudeController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PPPlugIns.h>

@interface AmplitudeController : NSWindowController
@property int amplitudeAmount;
@property NSRange selectionRange;
@property BOOL stereoMode;
@property (copy) PPPlugErrorBlock currentBlock;
@property (strong) PPSampleObject *theData;

@property (strong) NSWindow *parentWindow;

- (IBAction)okay:(id)sender;
- (IBAction)cancel:(id)sender;

@end
