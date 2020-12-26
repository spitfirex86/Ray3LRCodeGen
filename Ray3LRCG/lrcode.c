#include "framework.h"
#include "lrcode.h"

const char *MagicString = "134M568Q9BC+=DFNG7H-JKPW2RSTLVXZ";

int LumRace_WriteCode( char *lpDst, const BYTE *Magic, const int nChars )
{
	int idx = 0;

	for ( int i = 0; i < nChars; i++ )
	{
		if ( (idx & 3) == 3 )
		{
			lpDst[idx++] = ' ';
		}

		int magicValue = Magic[i];
		lpDst[idx++] = MagicString[magicValue];
	}
	lpDst[idx] = 0;

	return idx;
}

void LumRace_4( BYTE *Magic, int prevIdx, int Direction )
{
	BYTE *lpMagic;
	int idx;

	if ( Direction >= 0 )
	{
		lpMagic = Magic;
		idx = 0;
	}
	else
	{
		idx = prevIdx - 1;
		lpMagic = &Magic[idx];
		prevIdx = -1;
	}

	BS count1 = { prevIdx };
	BS count2 = { 0 };
	int value = 0;

	for ( char i = 0; idx != prevIdx; count2.dword = (value + count2.dword + 2) & 0x1F )
	{
		idx += Direction;
		count1.lobyte = count2.lobyte ^ i ^ *lpMagic;
		value = *lpMagic - Direction;
		i = count2.dword;
		*lpMagic = count1.lobyte;
		count1.dword = prevIdx;
		lpMagic += Direction;
	}
}

void LumRace_3( BYTE *Magic, int prevIdx )
{
	BYTE *lpMagic;

	LumRace_4(Magic, prevIdx, -1);

	if ( prevIdx - 1 > 0 )
	{
		lpMagic = Magic;
		do
		{
			int val = lpMagic[1] + 32 * lpMagic[0];
			val *= 0xABCD;

			lpMagic[0] = val & 0x1F;
			lpMagic[1] = (val >> 5) & 0x1F;

			lpMagic += 2;
		}
		while ( (int)(lpMagic - Magic) < prevIdx - 1 );
	}

	LumRace_4(Magic, prevIdx, 1);

	if ( prevIdx - 2 > 0 )
	{
		lpMagic = Magic;
		do
		{
			int v1 = lpMagic[1] + 32 * lpMagic[2];
			int v2 = lpMagic[0];
			int val = v2 + 32 * v1;
			val *= 0xABCD;

			lpMagic[1] = (val >> 10) & 0x1F;
			lpMagic[0] = (val >> 5) & 0x1F;
			lpMagic[2] = val & 0x1F;

			lpMagic += 3;
		}
		while ( (int)(lpMagic - Magic) < prevIdx - 2 );
	}

	LumRace_4(Magic, prevIdx, -1);

	if ( prevIdx - 3 > 0 )
	{
		lpMagic = Magic;
		do
		{
			int v1 = 0x1B3C5D7 * (lpMagic[0] + 32 * lpMagic[1]) - 0x1D148000 * lpMagic[3];
			int v2 = lpMagic[2];
			int val = v1 - 0x30E8A400 * v2;

			lpMagic[1] = (val >> 15) & 0x1F;
			lpMagic[3] = (val >> 10) & 0x1F;
			lpMagic[0] = (val >> 5) & 0x1F;
			lpMagic[2] = val & 0x1F;
			lpMagic += 4;
		}
		while ( (int)(lpMagic - Magic) < prevIdx - 3 );
	}

	LumRace_4(Magic, prevIdx, 1);
}

int LumRace_2( BYTE *Magic, int prevIdx, BYTE *Result, int SaveRnd )
{
	BYTE magicCopy[MAGIC_SIZE];
	memcpy(magicCopy, Magic, prevIdx);

	BYTE *lpMagic = Magic;
	int l1 = 0;
	int l2 = 0;
	int saveRnd2 = SaveRnd;

	if ( SaveRnd > 8 )
	{
		DWORD u1 = ((DWORD)(SaveRnd - 9) >> 3) + 1;
		DWORD u2 = u1;
		saveRnd2 = SaveRnd - 8 * u1;
		
		do
		{
			l2 |= (*Result << l1);
			l1 += 8;
			Result++;

			if ( l1 >= 5 )
			{
				DWORD u3 = l1 / 5u;
				l1 %= 5u;
				
				do
				{
					*lpMagic++ = l2 & 0x1F;
					l2 >>= 5;
					u3--;
				}
				while ( u3 );
				u1 = u2;
			}
			u2 = --u1;
		}
		while ( u1 );
	}

	int v1 = *Result << l1;
	int v2 = l1 + saveRnd2 + 4;
	int v3 = v1 | l2;

	if ( v2 >= 5 )
	{
		DWORD u4 = v2 / 5u;

		do
		{
			*lpMagic++ = v3 & 0x1F;
			v3 >>= 5;
			u4--;
		}
		while ( u4 );
	}

	memcpy(lpMagic, magicCopy, prevIdx);
	return prevIdx + (SaveRnd + 4) / 5;
}

int LumRace_1( BYTE *Magic, int prevIdx, BS *Result, int SaveRnd )
{
	int idx = LumRace_2(Magic, prevIdx, Result, SaveRnd);
	LumRace_3(Magic, idx);

	return idx;
}

int LumRace( char *lpDst, DWORD dwScore, DWORD dwCages, int lSaveRnd )
{
	BYTE magic[MAGIC_SIZE];
	BS calc = { 0 };
	BS result = { 0 };

	if ( dwCages > 60 )
		dwCages = 60;

	if ( dwScore > 3500000 )
		dwScore = 3500000;

	calc.dword = ((0xFF234567 * (lSaveRnd & 0xFFFFF)) & 0x1FFFFFF) ^ (5 * (dwCages + 61 * dwScore));

	result.lobyte = calc.byte1;
	result.byte1 = (103 * lSaveRnd) ^ (5 * (dwCages + 61 * dwScore));
	result.hiword = calc.hiword;

	int idx = LumRace_1(magic, 0, &result, 30); // should be 6

	result.loword = 0x4567 * lSaveRnd;
	result.hiword = ((0xFF234567 * (lSaveRnd & 0xFFFFF)) & 0x1FFFFFF) >> 16;

	idx = LumRace_1(magic, idx, &result, 25); // should be 11

	BYTE checksum = 0;
	for ( int i = 0; i < idx; i++ )
	{
		checksum += magic[i];
	}
	magic[idx] = checksum & 0x1F;

	return LumRace_WriteCode(lpDst, magic, idx + 1);
}