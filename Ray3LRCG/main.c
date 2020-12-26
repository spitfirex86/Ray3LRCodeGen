#include <stdlib.h>
#include <time.h>
#include "framework.h"
#include "resource.h"
#include "lrcode.h"

#define BUFFER_SIZE 128

HINSTANCE hInst;

HWND hPoints;
HWND hCages;
HWND hSaveRnd;
HWND hGenerate;
HWND hLumCode;

BOOL bGenEnabled;

BOOL DwordFromHwndText( HWND hWnd, DWORD *dwOut )
{
	char szBuffer[BUFFER_SIZE];
	int err = 0;

	GetWindowText(hWnd, szBuffer, BUFFER_SIZE);
	_set_errno(0);
	*dwOut = strtoul(szBuffer, NULL, 10);
	_get_errno(&err);

	if ( err )
		return FALSE;

	return TRUE;
}

void GenerateSaveRnd( void )
{
	char szRnd[16];
	int val = (rand() % 0x1000 << 12) + (rand() % 0x1000);
	sprintf_s(szRnd, 16, "%i", val);
	SetWindowText(hSaveRnd, szRnd);
}

void GenerateLumCode( void )
{
	char szLumCode[16];
	DWORD dwPoints;
	DWORD dwCages;
	DWORD dwSaveRnd;

	if ( DwordFromHwndText(hPoints, &dwPoints)
		&& DwordFromHwndText(hCages, &dwCages)
		&& DwordFromHwndText(hSaveRnd, &dwSaveRnd) )
	{
		LumRace(szLumCode, dwPoints, dwCages, dwSaveRnd);
		SetWindowText(hLumCode, szLumCode);
		GenerateSaveRnd();
	}
}

void UpdateGenButtonState( void )
{
	if ( GetWindowTextLength(hPoints)
		&& GetWindowTextLength(hCages)
		&& GetWindowTextLength(hSaveRnd) )
	{
		if ( !bGenEnabled )
		{
			EnableWindow(hGenerate, TRUE);
			bGenEnabled = TRUE;
		}
	}
	else
	{
		if ( bGenEnabled )
		{
			EnableWindow(hGenerate, FALSE);
			bGenEnabled = FALSE;
		}
	}
}

BOOL MainDlgCommandProc(
	HWND hWnd,
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch ( LOWORD(wParam) )
	{
	case IDC_POINTS:
	case IDC_CAGES:
	case IDC_SAVERND:
		if ( HIWORD(wParam) == EN_CHANGE )
		{
			UpdateGenButtonState();
			break;
		}
		return FALSE;

	case IDC_GENERATE:
		GenerateLumCode();
		break;

	case IDOK:
	case IDCANCEL:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK MainDialogProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	HICON hIcon;

	switch ( uMsg )
	{
	case WM_INITDIALOG:
		// Set caption and taskbar icon
		hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		// Get control handles
		hPoints = GetDlgItem(hWnd, IDC_POINTS);
		hCages = GetDlgItem(hWnd, IDC_CAGES);
		hSaveRnd = GetDlgItem(hWnd, IDC_SAVERND);
		hGenerate = GetDlgItem(hWnd, IDC_GENERATE);
		hLumCode = GetDlgItem(hWnd, IDC_LUMCODE);

		EnableWindow(hGenerate, FALSE);
		bGenEnabled = FALSE;
		GenerateSaveRnd();

		break;

	case WM_COMMAND:
		return MainDlgCommandProc(hWnd, wParam, lParam);

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	INT nCmdShow
	)
{
	HWND hDlg;
	MSG msg;
	hInst = hInstance;

	srand(GetTickCount());

	hDlg = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDialogProc);

	if ( hDlg == NULL )
		return 1;
	ShowWindow(hDlg, nCmdShow);

	while ( GetMessage(&msg, NULL, 0, 0) > 0 )
	{
		if ( !IsDialogMessage(hDlg, &msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}
