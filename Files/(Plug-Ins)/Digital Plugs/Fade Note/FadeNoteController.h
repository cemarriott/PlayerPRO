//
//  FadeNoteController.h
//  PPMacho
//
//  Created by C.W. Betts on 9/10/13.
//
//

#import <Cocoa/Cocoa.h>
#import <PlayerPROKit/PlayerPROKit.h>

@interface FadeNoteController : PPDigitalPluginWindowController
@property (weak) IBOutlet NSComboBox *fromBox;
@property (weak) IBOutlet NSComboBox *toBox;

@end