#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include <stdio.h>

	extern	EventRecord				theEvent;
	extern	Cursor					HandCrsr;
	extern	MenuHandle				ViewsMenu;
	
			DialogPtr				CubeDlog;

void DoInternetMenu(short theItem);

void  UpdateCubeWindow(DialogPtr GetSelection)
{
	GrafPtr		SavePort;
	
	GetPort(&SavePort);
	SetPortDialogPort(CubeDlog);

	BeginUpdate(GetDialogWindow(CubeDlog));

	DrawDialog(CubeDlog);

	EndUpdate(GetDialogWindow(CubeDlog));

	SetPort(SavePort);
} 

void CreateCubeWindow(void)
{
	Rect		itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp, i;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		String;
	GrafPtr		savePort;

	if (CubeDlog != NULL)
	{
		SelectWindow2(GetDialogWindow(CubeDlog));
		return;
	}
	
	CubeDlog = GetNewDialog(132, NULL, GetDialogWindow(ToolsDlog));
	
//	SetWindEtat(CubeDlog);
	SetPortDialogPort(CubeDlog);
	
	ShowWindow(GetDialogWindow(CubeDlog));
	SelectWindow2(GetDialogWindow(CubeDlog));
}

void CloseCube(void)
{
	if (CubeDlog != NULL)
	{
		DisposeDialog(CubeDlog);
	}
	CubeDlog = NULL;
	
//	SetItemMark(ViewsMenu, m3D, noMark);
}

void DoItemPressCube(short whichItem, DialogPtr whichDialog)
{
	switch (whichItem)
	{
		case 1:
			DoInternetMenu(1);
		break;
	}
}
