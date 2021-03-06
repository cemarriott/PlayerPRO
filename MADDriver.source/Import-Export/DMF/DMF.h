/********************						***********************/
//
//	Player PRO 5.0 - DRIVER SOURCE CODE -
//
//	Library Version 5.0
//
//	To use with MAD Library for Mac: Symantec, CodeWarrior and MPW
//
//	Antoine ROSSET
//	16 Tranchees
//	1206 GENEVA
//	SWITZERLAND
//
//	COPYRIGHT ANTOINE ROSSET 1996, 1997, 1998
//
//	Thank you for your interest in PlayerPRO !
//
//	FAX:				(+41 22) 346 11 97
//	PHONE: 			(+41 79) 203 74 62
//	Internet: 	RossetAntoine@bluewin.ch
//
/********************						***********************/


#pragma pack(push, 2)

typedef struct ITPatForm {
	unsigned short	length;
	unsigned short	row;
	char			no[4];
	char			data[];
} ITPatForm;

typedef struct ITSampForm {
	MADFourChar	ID;
	char		DOSName[12];
	char		no;
	char		GvL;
	char		Flag;
	char		Vol;
	char		SampName[26];
	short		Convert;
	int			length;
	int			loopBegin;
	int			loopEnd;
	int			C5Speed;
	int			SusLoopBegin;
	int			SusLoopEnd;
	int			samplePtr;
	char		ViS;
	char		ViD;
	char		ViR;
	char		ViT;
} ITSampForm;

typedef struct ITNode {
	char	y;
	short	x;
} ITNode;

typedef struct ITEnv {
	char	Flag;
	char	Num;
	char	LpB;
	char	LpE;
	char	SLB;
	char	SLE;
	ITNode	nodes[];
} ITEnv;

typedef struct ITInsForm
{
	int		ID;
	char	DOSName[12];
	char	no;
	char	NNA;
	char	DCT;
	char	DCA;
	short	FadeOut;
	char	PPS;
	char	PPC;
	char	GbV;
	char	DfP;
	char	no2[2];
	short	TrkVers;
	char	NoS;
	char	no3;
	char	INSName[26];
	char	no4[6];
	char	keyMap[240];
	ITEnv	volEnv;
	ITEnv	panEnv;
	ITEnv	pitchEnv;
} ITInsForm;

//TODO: check pointer values use in files
typedef struct ITForm
{
	MADFourChar		ID;
	char			name[26];
	char			no[2];
	
	short			orderNum;
	short			insNum;
	short			smpNum;
	short			patNum;
	short			cwtv;
	short			Cmwt;
	short			flags;
	short			special;
	
	char			globalVol;
	char			mixVol;
	char			iSpeed;
	char			iTempo;
	char			panSeparation;
	char			null;
	short			MsgLgth;
	int				MsgOffset;
	char			no2[4];
	
	char			chanPan[64];
	char			chanVol[64];
	
	unsigned char	*orders;
	int				*parapins;
	int				*parapsamp;
	int				*parappat;
	
	ITInsForm		*insdata;
	ITSampForm		*sampdata;
} ITForm;

#pragma pack(pop)
