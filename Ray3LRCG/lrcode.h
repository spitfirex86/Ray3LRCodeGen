#pragma once

#include "framework.h"

#define MAGIC_SIZE 256

typedef union
{
	DWORD dword;

	struct
	{
		WORD loword;
		WORD hiword;
	};

	struct
	{
		BYTE lobyte;
		BYTE byte1;
		BYTE byte2;
		BYTE hibyte;
	};
} BS;

int LumRace( char *lpDst, DWORD dwScore, DWORD dwCages, int lSaveRnd );
