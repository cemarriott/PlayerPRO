#include <Carbon/Carbon.r>

resource 'DITL' (128) {
	{	/* array DITLarray: 15 elements */
		/* [1] */
		{20, 140, 37, 198},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{50, 140, 67, 198},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{150, 60, 166, 92},
		EditText {
			enabled,
			""
		},
		/* [4] */
		{180, 60, 196, 92},
		EditText {
			enabled,
			""
		},
		/* [5] */
		{150, 10, 166, 54},
		StaticText {
			disabled,
			"From:"
		},
		/* [6] */
		{180, 10, 196, 54},
		StaticText {
			disabled,
			"To:"
		},
		/* [7] */
		{10, 10, 28, 116},
		RadioButton {
			enabled,
			"Instrument"
		},
		/* [8] */
		{30, 10, 48, 116},
		RadioButton {
			enabled,
			"Note"
		},
		/* [9] */
		{50, 10, 68, 116},
		RadioButton {
			enabled,
			"Argument"
		},
		/* [10] */
		{70, 10, 88, 116},
		RadioButton {
			enabled,
			"Volume"
		},
		/* [11] */
		{112, 60, 128, 92},
		EditText {
			enabled,
			"1"
		},
		/* [12] */
		{112, 10, 128, 54},
		StaticText {
			disabled,
			"Step:"
		},
		/* [13] */
		{100, -30, 101, 322},
		Picture {
			disabled,
			11
		},
		/* [14] */
		{140, -40, 141, 312},
		Picture {
			disabled,
			11
		},
		/* [15] */
		{150, 100, 166, 200},
		StaticText {
			disabled,
			""
		}
	}
};

data 'DLGX' (128) {
	$"0843 6861 7263 6F61 6C00 0000 0000 0000"            /* .Charcoal....... */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"000C 0000 0000 0000 0004 0004 0000 0000"            /* ................ */
	$"000F 0000 0000 0000 0000 0000 0000 0001"            /* ................ */
	$"0000 0000 0000 0000 0000 0007 0000 0000"            /* ................ */
	$"0000 0000 0000 0007 0000 0000 0000 0000"            /* ................ */
	$"0000 0006 0000 0000 0000 0000 0000 0006"            /* ................ */
	$"0000 0000 0000 0000 0000 0004 0000 0000"            /* ................ */
	$"0000 0000 0000 0004 0000 0000 0000 0000"            /* ................ */
	$"0000 0004 0000 0000 0000 0000 0000 0004"            /* ................ */
	$"0000 0000 0000 0000 0000 0007 0000 0000"            /* ................ */
	$"0000 0000 0000 0006 0000 0000 0000 0000"            /* ................ */
	$"0000 0009 0000 0000 0000 0000 0000 0009"            /* ...�...........� */
	$"0000 0000 0000 0000 0000 0006 0000 0000"            /* ................ */
	$"0000 0000 0000"                                     /* ...... */
};

resource 'DLOG' (128) {
	{84, 134, 290, 342},
	movableDBoxProc,
	invisible,
	goAway,
	0x0,
	128,
	"Volume Fade",
	centerMainScreen
};

resource 'STR#' (1000) {
	{	/* array StringArray: 1 elements */
		/* [1] */
		"Complex Fade"
	}
};

resource 'dctb' (128) {
	{	/* array ColorSpec: 5 elements */
		/* [1] */
		wContentColor, 56797, 56797, 56797,
		/* [2] */
		wFrameColor, 0, 0, 0,
		/* [3] */
		wTextColor, 0, 0, 0,
		/* [4] */
		wHiliteColor, 0, 0, 0,
		/* [5] */
		wTitleBarColor, 65535, 65535, 65535
	}
};

data 'ictb' (128) {
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000"            /* ................ */
	$"0000 0000 0000 0000 0000 0000"                      /* ............ */
};
