
/* Command.c */
void SwitchSize(void);
void SizeCommandWindow(Boolean, Boolean);
Cmd *GetCmdDlogActif(void);
void ShowCurrentCmdNote(void);
void UpdatePatternMenu(void);
void AfficheGoodEffect(short, Byte);
void SetCommandTrack(int, long);
short Analyse(short);
void SetCmdValue(short);
void myTESetText(Ptr, long, TEHandle);
void DoNullCmdWindow(void);
void UpdateCmdDlogInfo(void);
void UpdateCmdDlogWindow(DialogPtr);
void PlayCurNote(void);
void DoItemPressCmdDlog(short, DialogPtr);
void CreateCmdDlog(void);
void CloseCmdDlog(void);
Boolean TakeDecimal(short *);
Boolean HexaDecimal(short *);
Boolean TakeNote(short *);
Boolean AcceptKeysTools(void);
void DoKeyPressCmdDlog(short);
void UpdateCurrentCmd(void);
void ApplyBut(short);
void DesactivateCmdWindow(void);