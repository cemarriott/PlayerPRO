# PlayerPRO

PlayerPRO is a SoundTracker player and creator for the Mac. It should be able to play XM, IT, MOD, S3M, and other formats. 

The PlayerPROCore Framework is the main playback mechanism of PlayerPRO (Previously known as MADDriver) that allows developers to play SoundTracker audio in their applications.

## PlayerPRO 6

PlayerPRO 6 is a rewrite of the PlayerPRO to help make it work better with different operating systems, as well as update the program to use Cocoa.  Please note that this is not a stable branch and _**will**_ break backwards compatibility. The framework itself runs on 10.7 and later. It is not Garbage-Collection capable.

There is a new framework, PlayerPROKit, that wraps many PlayerPROCore functions and data types as methods of Objective-C classes. It is 64-bit on OS X and requires 10.9 or later.

The application has been split into two different apps: PlayerPRO Player and PlayerPRO 6. PlayerPRO Player allows playback of tracker files, as well as having a playlist. PlayerPRO 6 allows editing of tracker files. Both require 10.9 or later.

Plug-ins now depend on the PlayerPROCore library directly instead of having the PlayerPRO functions looked up when loading. The previous method was needed for the Carbon app, but will not be needed for the Cocoa re-write.

### Deprecations

Most Carbon API usage has been removed. Most notably, the memory functions have been changed from Carbon to POSIX. The Carbon File APIs have been replaced with POSIX. `iFileOpen` has been deprecated, and the replacements `iFileOpenRead` and `iFileOpenWrite` must be sent either a full path or a path relative to the current working directory: using `HSetVol` will no longer affect the directory that `iFileOpenRead` or `iFileOpenWrite` uses as its base. There are still functions to open a file using `FSSpec`s, as well as using `FSRef`s, but these are deprecated, with the `FSSpec` functions not working on 64-bit code. Functions that use `CFURL`s are available on OS X and iOS.

Sound Manager support has been removed: use the CoreAudio implementation instead.

The instrument plug-ins now use `CFURL`s instead of the deprecated `FSSpec`s. 

### Platform Support

iOS support has been added, but due to limitations in the iOS SDK, plug-ins are not supported: instead the default plug-ins are built-into the static library. 

Windows support is available.

BeOS code has been recovered, but has not been tested.

Linux support has been added, but the makefile is bare-bones and plug-ins are currently not being built.

OS X tracker plug-ins no longer uses the CFPlugInCom code: instead, they must implement the function `PPImpExpMain` and export it as a C symbol. The other plug-in architectures also now use this symbol instead of `mainPLUG`. In order to make a tracker plug-in on an architecture other than OS X, the plug-in must implement and export the C symbol `FillPlug` to fill out the plug-in name and other metadata.

### Additional Notes

Refrain from accessing the `MADDriverRec` struct directly: the size may differ from the compiled version due to differences in preprocessor macros. This will most likely to be an issue on Linux/UNIX platforms.

## Installation

To install this on Unix derivatives like Linux, follow the instructions in **INSTALL**. Note that Unix support is lacking at the moment.

To build the PlayerPRO application and library on OS X, open the PPMacho project or the PlayerPRO Workspace.

If you wish to build PlayerPROCore on Windows, you will have to use the same `Cmake` program as the Linux users.
