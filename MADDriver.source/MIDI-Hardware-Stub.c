#ifdef MAINPLAYERPRO
#include "Shuddup.h"
#endif
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "PPPrivate.h"

#include <stdio.h>

#define MySignature		'SNPL'

void DoPlayInstruInt(short Note, short Instru, short effect, short arg, short vol, Channel *curVoice, int start, int end);
void NPianoRecordProcess(short i, short, short, short);
void OpenOrCloseConnection(Boolean opening);

void MyNullHook()
{
	
}

void CloseMIDIHarware(void)
{

}

void OpenMIDIHardware(MADDriverRec *rec)
{

}

void InitMIDIHarware(void)
{
	//MIDIHardware = false;
	//MIDIHardwareAlreadyOpen = false;
}

void NDoPlayInstru(short Note, short Instru, short effect, short arg, short vol);

void DoMidiSpeaker(short note, short Instru, SInt32 arg)
{

}

void OpenOrCloseConnection(Boolean opening)
{

}

void SelectOMSConnections(Boolean Input)
{

}

void SendMIDIClock(MADDriverRec *intDriver, Byte MIDIByte)
{

}

void SendMIDITimingClock(MADDriverRec *MDriver)
{

}