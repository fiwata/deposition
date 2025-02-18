#ifndef CDIALOG_H
#define CDIALOG_H
#include <Windows.h>
#include "Resource.h"
#include "preferencemanager.h"
static void SetEditText(HWND hDlg)
{
	HWND hEdit = GetDlgItem(hDlg, INFO_LISTING_DLG);
	if (hEdit)
	{
		const char* initialText = "Some settings\r\n";
		const char* virtualStatus = (getprefString(MENU_SUB_VIR_REA) == "ON") ? "\r\n- Virtual DAQ is On" : "\r\n- REAL DAQ is On";
		const char* upperth = "\r\n- Upper Threshold value: ";
		const std::string upp = getprefString(UTH_KEY);
		const char* upperTHV = upp.c_str();


		const char* stage = (getprefString(STAG_TOGGLE_KEY) == "on") ? "\r\n- STAG_TOGGLE_KEY is On" : "\r\n- STAG_TOGGLE_KEY is OFF";
		const char* threshtogg = (getprefString(THRESHOLD_TOGGLE_KEY) == "on") ? "\r\n- THRESHOLD_TOGGLE_KEY is On" : "\r\n- THRESHOLD_TOGGLE_KEY is OFF";
		const char* stagex = (getprefString(STAG_X_TOGGLE_KEY) == "on") ? "\r\n- STAG_X_TOGGLE_KEY is On" : "\r\n- STAG_X_TOGGLE_KEY is OFF";
		const char* stagey = (getprefString(STAG_Y_TOGGLE_KEY) == "on") ? "\r\n- STAG_Y_TOGGLE_KEY is On" : "\r\n- STAG_Y_TOGGLE_KEY is OFF";
		const char* stagez = (getprefString(STAG_Z_TOGGLE_KEY) == "on") ? "\r\n- STAG_Z_TOGGLE_KEY is On" : "\r\n- STAG_Z_TOGGLE_KEY is OFF";

		// Format the concatenated string
		char concatenatedText[512]; // Adjust the size as needed
		sprintf(concatenatedText, "%s %s", initialText, virtualStatus);
		
		strcat(concatenatedText, upperth);
		strcat(concatenatedText, upperTHV);
		strcat(concatenatedText, stage);
		strcat(concatenatedText, threshtogg);
		strcat(concatenatedText, stagex);
		strcat(concatenatedText, stagey);
		strcat(concatenatedText, stagez);
		// Set the concatenated text as the edit control text
		SetWindowTextA(hEdit, concatenatedText);
	}
}

static INT_PTR CALLBACK Cdialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetEditText(hDlg);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;

}




#endif