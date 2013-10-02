
/* Utils.c */
void NNumToString(short, Str255);
void oldFrameButton(DialogPtr);
void CenterRect(Rect *, Rect *, Rect *);
pascal Boolean mylClickLoop(void);
pascal Boolean MyDlgFilter(DialogPtr, EventRecord *, short *);
void MyUpdateDialog(DialogPtr);
Boolean MyIntModalDialog(DialogPtr, short *, EventRecord *);
Boolean MyModalDialog(DialogPtr, short *);
pascal Boolean MyDlgFilterSF(DialogPtr, EventRecord *, short *, void *);
void ToolBoxInit(void);
void InverseRadio(short, DialogPtr);
void TurnRadio(short, DialogPtr, Boolean);
void ControlSwitch(short, DialogPtr, short);
void MyMoveControl(ControlHandle, short, short);
void MySizeControl(ControlHandle, short, short);
void MySizeWindow(DialogPtr, short, short, Boolean);
Boolean MyTrackControl(ControlHandle, Point, ControlActionUPP);
short NewOffscreenBitMap(BitMap *, Rect *);
void ZapBitMap(BitMap *);
void BitToPic(PicHandle *, BitMap, Rect *);
void SetDText(DialogPtr, short, Str255);
void WriteCText(DialogPtr, short, char *);
void GetDText(DialogPtr, short, StringPtr);
void OSType2Str(OSType, Str255);
void ByteSwapMADSpec(MADSpec *toSwap);
void ByteSwapsData(sData *toSwap);
void ByteSwapInstrData(InstrData *toSwap);
void ByteSwapPatHeader(PatHeader *toSwap);
void pStrcpy(register unsigned char *, register const unsigned char *);
void pStrcat(register unsigned char *, register unsigned char *);
void ErasePixMap(PixMapHandle);
short NewOffscreenPixMap(PixMapHandle *, Rect *);
Ptr NewADDRPtrI(short);
void ZapPixMap(PixMapHandle *);
void PixToPic(PicHandle *, PixMapHandle, Rect);
short PicToPix(PicHandle, PixMapHandle *);
short IclToPix(Handle, PixMapHandle *);
unsigned short RangedRdm(unsigned short, unsigned short);
void DessineRgn(Rect *, Rect *);
void FrameButton(DialogPtr, short, Boolean);
short SetScroll(ControlHandle, TEHandle);
void FrameRectRelief(Rect *);