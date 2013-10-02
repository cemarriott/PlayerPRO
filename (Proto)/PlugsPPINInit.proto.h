
/* PlugsPPINInit.c */
OSErr CallPPINPlugIns(short, OSType, short, short *, FSSpec *);
void LoadPPINPLUG(short, StringPtr);
void ScanDirPPINPlug(long, short);
void InitPPINMenu(void);
void GetSampInstPPIN(short *, short *);
OSErr PPINGetPlugByID(OSType *, short, short);
OSType PressPPINMenu(Rect *, OSType, short, Str255);
void InitPPINPlug(void);
short PPINGetNo(void);
OSErr PPINGetPlugName(OSType, Str255);
OSErr PPINPlayFile(OSType, FSSpec *);
OSErr PPINTestFile(OSType, FSSpec *);
OSErr PPINAvailablePlug(OSType, OSType *);
OSErr PPINIdentifyFile(OSType *, FSSpec *);
OSErr PPINImportFile(OSType, short, short *, FSSpec *);
OSErr PPINExportFile(OSType, short, short, FSSpec *);
void InitPlayWhenClicked(void);
pascal Boolean PPINCustomFileFilter(ParmBlkPtr, void *);
void PPINWriteSupportedFormat(DialogPtr);
pascal short PPINDlgHook(short, DialogPtr, void *);
void PPINGetFileName(void);