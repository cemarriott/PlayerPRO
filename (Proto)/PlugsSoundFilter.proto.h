
/* PlugsSoundFilter.c */
OSErr NCallPlugIns(short, sData *, long, long);
void LoadPLUGSE(short, StringPtr);
void LoadPLUG(short, StringPtr);
void ScanDirPlug(long, short);
void InitSampleMenu(void);
short PressSampleMenu(Rect *);
void InitPlug(void);