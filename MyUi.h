#ifndef MY_UI_H
#define MY_UI_H
#include <Windows.h>
#include "Resource.h"
#include <iostream>
#include <CommCtrl.h>
#include "preferencemanager.h"

class MyUI {
public:
	HWND hCombo;
	HWND btnUth,btnLth,btnPZT,btnSQH,btnSQW,btnSQX,btnSQY,btnTIME;
	HWND txtBD, txtEVOLT,txtPZT,txtTIME,txtUTH,txtLTH, btnCAMINDEX, btnMSQX1, btnMSQY1, btnMSQX2, btnMSQY2,fileCSV;
	HWND hFrame,camcam;

	std::string onoff;

	static HWND MyUI::CreateListView(int x, int y, int width, int height, HWND parent) {
		//CreateButton(L"Save", x, y, width, height, parent, NULL);
		return CreateWindow(WC_LISTVIEW, "", WS_VISIBLE | WS_CHILD | LVS_REPORT, x, y, width, height, parent, NULL, NULL, NULL);
	}
	static HWND MyUI::CreateButton(const wchar_t* text, int x, int y, int btw, int bth, HWND parent, int id, DWORD style = BS_PUSHBUTTON)
	{
		// Create the button
		HWND button = CreateWindowW(L"BUTTON", text, WS_VISIBLE | WS_CHILD | style, x, y, btw, bth, parent, (HMENU)id, NULL, NULL);
		return button;
	}

	static HWND MyUI::CtBtn(const wchar_t* text, int x, int y, int btw, int bth, HWND parent, int id, std::string key)
	{
		HWND boolrecord = CreateWindowW(L"BUTTON", text, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, btw, bth, parent, (HMENU)id, NULL, NULL);
		SendMessage(boolrecord, getprefString(key) == "on" ? BM_SETCHECK : BM_SETCHECK, getprefString(key) == "on" ? BST_CHECKED : BST_UNCHECKED, 0);
		// Use BS_AUTOCHECKBOX style for toggle button
		return boolrecord;
	}


	static HWND MyUI::CreateStaticText(const wchar_t* text, int x, int y, int width, int height, HWND parent, int id, DWORD style= WS_BORDER)
	{
		return CreateWindowW(L"STATIC", text, WS_VISIBLE | WS_CHILD | style, x, y, width, height, parent, (HMENU)id, NULL, NULL);
	}

	static HWND MyUI::InputSaveButton(const wchar_t* inputText, int x, int y, int w, int h, const wchar_t* buttonText, HWND parent, int inputId, int buttonId, DWORD inputStyle = WS_BORDER, DWORD buttonStyle = BS_PUSHBUTTON, int ratio = 50)
	{
		HWND hInput;
		if (ratio == 50) {
			hInput = CreateWindowW(L"EDIT", inputText, WS_VISIBLE | WS_CHILD | inputStyle, x, y, w / 2, h, parent, (HMENU)inputId, NULL, NULL);
			HWND hButton = CreateWindowW(L"BUTTON", buttonText, WS_VISIBLE | WS_CHILD | buttonStyle, x + w / 2, y, w / 2, h, parent, (HMENU)buttonId, NULL, NULL);
		}
		else {
			hInput = CreateWindowW(L"EDIT", inputText, WS_VISIBLE | WS_CHILD | inputStyle, x, y, w * 0.75, h, parent, (HMENU)inputId, NULL, NULL);
			HWND hButton = CreateWindowW(L"BUTTON", buttonText, WS_VISIBLE | WS_CHILD | buttonStyle, x + w * 0.75, y, w * 0.25, h, parent, (HMENU)buttonId, NULL, NULL);
		}
		return hInput;
	}

	std::string MyUI::GetInputText(HWND hInput)
	{
		// Get the length of the text
		int textLength = GetWindowTextLengthW(hInput) + 1;

		// Allocate a buffer to hold the text
		wchar_t* buffer = new wchar_t[textLength];

		// Get the text from the input control
		GetWindowTextW(hInput, buffer, textLength);

		// Convert to std::wstring
		std::string result(buffer, buffer + textLength);
		// Clean up
		delete[] buffer;

		return result;
	}

	std::string MyUI::getTextInput(HWND hInput)
	{
		// Get the length of the text
		int textLength = GetWindowTextLengthW(hInput) + 1;

		// Allocate a buffer to hold the text
		wchar_t* buffer = new wchar_t[textLength];

		// Get the text from the input control
		GetWindowTextW(hInput, buffer, textLength);

		// Convert to std::wstring
		std::string result(buffer, buffer + textLength);
		// Clean up
		delete[] buffer;

		return result;
	}

	HWND MyUI::mainUi(HWND hWnd) {
		const int x = std::round(0.50 * GetSystemMetrics(SM_CXSCREEN));
		const int y = std::round(0.10 * GetSystemMetrics(SM_CYSCREEN));
		const int w = std::round(0.50 * GetSystemMetrics(SM_CXSCREEN));
		const int h = std::round(0.85 * GetSystemMetrics(SM_CYSCREEN));

		const int btw = std::round(0.07 * GetSystemMetrics(SM_CXSCREEN));
		const int bth = std::round(0.04 * GetSystemMetrics(SM_CYSCREEN));
		const int btspace = bth + 2;
		const int row2 = btw + 2;
		const int row3 = 2 * btw + 4;
		const int row4 = 3 * btw + 8;

		const int rowheight = std::round(0.45 * GetSystemMetrics(SM_CYSCREEN));

		hFrame = CreateButton(L"Menu", 0, 0, w, h, hWnd, NULL, BS_GROUPBOX);


		CreateButton(L"Camera ON", 0, 0, btw, bth, hFrame, ID_BTN_CAMERA_ON);
		CreateButton(L"Camera OFF", 0, btspace, btw, bth, hFrame, ID_BTN_CAMERA_OFF);
		CreateButton(L"Laser ON", 0, 2 * btspace, btw, bth, hFrame, ID_BTN_LASER_ON);
		CreateButton(L"Deposition ON", 0, 3 * btspace, btw, bth, hFrame, ID_BTN_DEPOSITION_ON);
		CreateButton(L"Dep. OFF", 0, 4 * btspace, btw, bth, hFrame, ID_BTN_DEPOSITION_OFF);
		hCombo = CreateButton(L"Setting", 0, 5 * btspace, btw, bth, hFrame, ID_CAMERA_OPTION);
		CreateButton(L"EPV0", 0, 6 * btspace, btw, bth, hFrame, ID_BTN_EPDV0);
		CreateButton(L"PZTV0", 0, 7 * btspace, btw, bth, hFrame, ID_BTN_PZTV0);
		CtBtn(L"EBOX", 0, 8 * btspace, btw, bth, hFrame, TGL_BTN_RBOX, EBOXONOFF_KEY);

		CtBtn(L"Auto-Graph", 0, 9 * btspace, btw, bth, hFrame, TGL_BTN_GRAPH, AUTOGRAPH_KEY);

		CtBtn(L"Auto-REcord", 0, 10 * btspace, btw, bth, hFrame, TGL_BTN_RECORD, AUTORECORD_KEY);
		
		btnUth = InputSaveButton(L"0", 0, 11 * btspace, btw, bth, L"Uth", hFrame, INPUT_UTH, BTN_UTH, NULL);
		loadPrefv(btnUth, UTH_KEY);
		btnLth = InputSaveButton(L"0", 0, 12 * btspace, btw, bth, L"Lth", hFrame, INPUT_LTH, BTN_LTH, NULL);
		loadPrefv(btnLth, LTH_KEY);
		btnPZT = InputSaveButton(L"0", 0, 13 * btspace, btw, bth, L"PZT", hFrame, INPUT_PZT, BTN_PZT, NULL);
		loadPrefv(btnPZT, PZT_KEY);
		btnTIME = InputSaveButton(L"0", 0, 14 * btspace, btw, bth, L"Time", hFrame, INPUT_TIME, BTN_TIME, NULL);
		loadPrefv(btnTIME, TIME_KEY);
		CtBtn(L"SD-DIFF", 0, 15 * btspace, btw, bth, hFrame, TGL_BTN_SDORDIFF, ADORDIFF_KEY);


		CreateButton(L"Camera RE", 0, 16 * btspace, btw, bth, hWnd, ID_BTN_CAMERA_RE);
		

		btnCAMINDEX = InputSaveButton(L"0", 0, 17 * btspace, 5 * btw, bth, L"Camera Index", hFrame, CAM_INDEX_INPUT, CAM_INDEX_BTN, WS_BORDER, BS_PUSHBUTTON, 90);
		loadPrefv(btnCAMINDEX, CameraIndex);
		fileCSV = InputSaveButton(L"0", 0, 18 * btspace, 4 * btw, bth, L"XYZ csv", hFrame, XYZ_CSV_INPUT, XYZ_CSV_BROWSE, WS_BORDER, BS_PUSHBUTTON, 90);
		loadPrefv(fileCSV, XYZ_CSV_FILENAME);

		//check folder and create if not allout\20240305 folder inside folder
		std::string folderName = "allout";
		std::string ddaa = GetYYMMDD();
		if (CheckAndCreateFolder(folderName, ddaa) && CheckAndCreateVideoFolder(folderName, ddaa)) {
			///
		}

		CtBtn(L"Stage T", row2, 0, btw, bth, hFrame, STAG_TOGGLE, STAG_TOGGLE_KEY);
		
		CtBtn(L"Threshold T", row2, btspace, btw, bth, hFrame, THRESHOLD_TOGGLE, THRESHOLD_TOGGLE_KEY);
		CtBtn(L"X stop", row2, 2 * btspace, btw, bth, hFrame, STAG_X_TOGGLE, STAG_X_TOGGLE_KEY);
		CtBtn(L"Y stop", row2, 3 * btspace, btw, bth, hFrame, STAG_Y_TOGGLE, STAG_Y_TOGGLE_KEY);
		CtBtn(L"Z stop", row2, 4 * btspace, btw, bth, hFrame, STAG_Z_TOGGLE, STAG_Z_TOGGLE_KEY);
		CtBtn(L"1/0 Cam", row2, 5 * btspace, btw, bth, hFrame, CAM_1_0_, CAM_1_0_KEY);
		CreateButton(L"CamaC", row2, 6 * btspace, btw, bth, hFrame, ID_BTN_CAMERA_CAMAC);
		CreateButton(L"Screenshot", row2, 7 * btspace, btw, bth, hFrame, TAKE_SCREENSHOT);



		camcam = CreateButton(L"This is camera area", 1.2*row3, 0, 10 * btw, 18 * bth, hWnd, NULL, BS_GROUPBOX);

		return hFrame;
	}
	static void always_run() {
		if (getprefString(MENU_SUB_VIR_REA) == "ON") {
			std::string tex = "Virtual is :on";
			SetPreference(HK_ERROR, tex);
		}
		if (getprefString(MENU_SUB_VIR_REA) == "OFF") {
			std::string tex = "Real is :on";
			SetPreference(HK_ERROR, tex);
		}

	}
	std::string MyUI::GetYYMMDD() {
		SYSTEMTIME st;
		GetLocalTime(&st);

		char formattedDateTime[7];  // 2 digits for year, 2 for month, 2 for day, plus 1 for null terminator
		_snprintf_s(formattedDateTime, sizeof(formattedDateTime), _TRUNCATE, "%02d%02d%02d",
			st.wYear % 100, st.wMonth, st.wDay);

		return std::string(formattedDateTime);
	}
	std::string MyUI::yymmdd_hhmmss() {
		SYSTEMTIME st;
		GetLocalTime(&st);

		char formattedDateTime[14];  // 2 digits for year, 2 for month, 2 for day, plus 1 for null terminator
		_snprintf_s(formattedDateTime, sizeof(formattedDateTime), _TRUNCATE, "%02d%02d%02d_%02d%02d%02d",
			st.wYear % 100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		return std::string(formattedDateTime);
	}

	static void MyUI::loadPref(HWND hwnd, std::string key) {
		std::stringstream newTextStream;
		newTextStream << key << ":" << getprefString(key);
		std::string text = newTextStream.str();
		std::wstring newText(text.begin(), text.end());
		SetWindowTextW(hwnd, newText.c_str());
	}
	static void MyUI::loadPrefv(HWND hwnd, std::string key) {
		std::stringstream newTextStream;
		newTextStream << getprefString(key);
		std::string text = newTextStream.str();
		std::wstring newText(text.begin(), text.end());
		SetWindowTextW(hwnd, newText.c_str());
	}


	HWND MyUI::getTxtBD() const {
		return txtBD;
	}
	HWND MyUI::getCamCam() const {
		return camcam;
	}
	HWND MyUI::getTxtUth() const {
		return txtUTH;
	}
	HWND MyUI::getTxtLth() const {
		return txtLTH;
	}
	HWND MyUI::getTxtEVOLT() const {
		return txtEVOLT;
	}
	HWND MyUI::getTxtPZT() const {
		return txtPZT;
	}
	HWND MyUI::getTxtTIME() const {
		return txtTIME;
	}


	HWND MyUI::getInputUth() const {
		return btnUth;
	}
	HWND MyUI::getInputlth() const {
		return btnLth;
	}
	HWND MyUI::getInputPZT() const {
		return btnPZT;
	}
	HWND MyUI::getInputSQH() const {
		return btnSQH;
	}
	HWND MyUI::getInputSQW() const {
		return btnSQW;
	}
	HWND MyUI::getInputSQX() const {
		return btnSQX;
	}
	HWND MyUI::getInputSQY() const {
		return btnSQY;
	}
	HWND MyUI::getInputTIME() const {
		return btnTIME;
	}

	static void MyUI::pushh(HWND hdmi, int input, std::string key) {
		wchar_t buffer[256];
		UINT textLength = GetDlgItemTextW(hdmi, input, buffer, sizeof(buffer) / sizeof(buffer[0]));
		if (textLength > 0) {
			wchar_t* text = buffer;
			SetPreferenceW(key, text);
		}
		else {
			wchar_t* text = L"e";
			SetPreferenceW(key, text);
		}
	}

	static INT_PTR CALLBACK MyUI::CameraOptions(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(lParam);
		HWND hubtn = GetDlgItem(hDlg, ST_INP_UTH);
		HWND hlbtn = GetDlgItem(hDlg, ST_INP_LTH);
		HWND hpbtn = GetDlgItem(hDlg, ST_INP_PZT);
		HWND hebtn = GetDlgItem(hDlg, ST_INP_EPV);
		HWND htbtn = GetDlgItem(hDlg, ST_INP_TIME);
		HWND sqwval = GetDlgItem(hDlg, ST_INP_SQW);
		HWND sqhval = GetDlgItem(hDlg, ST_INP_SQH);
		HWND sqx1p = GetDlgItem(hDlg, ST_INP_SQX1);
		HWND sqy1p = GetDlgItem(hDlg, ST_INP_SQY1);
		HWND sqXX1 = GetDlgItem(hDlg, ST_INP_XX1);
		HWND sqYY1 = GetDlgItem(hDlg, ST_INP_YY1);
		HWND sqXX2 = GetDlgItem(hDlg, ST_INP_XX2);
		HWND sqYY2 = GetDlgItem(hDlg, ST_INP_YY2);
		HWND path = GetDlgItem(hDlg, ST_INP_CURRENT_PATH);
		HWND err = GetDlgItem(hDlg, ST_INP_ERR);
		always_run();
		switch (message)
		{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);

			switch (wmId)
			{
			case (IDC_CANCEL):
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			case IDC_APPLY:
				loadPrefv(hubtn, UTH_KEY);
				loadPrefv(hlbtn, LTH_KEY);
				loadPrefv(hpbtn, PZT_KEY);
				loadPrefv(hebtn, EPV_KEY);
				loadPrefv(htbtn, TIME_KEY);
				loadPrefv(sqwval, SQW_KEY);
				loadPrefv(sqhval, SQH_KEY);
				loadPrefv(sqx1p, SQX1_KEY);
				loadPrefv(sqy1p, SQY1_KEY);
				loadPrefv(sqXX1, MSQX1_KEY);
				loadPrefv(sqYY1, MSQY1_KEY);
				loadPrefv(sqXX2, MSQX2_KEY);
				loadPrefv(sqYY2, MSQY2_KEY);
				loadPrefv(path, CURRENT_FOLDER);
				loadPrefv(err, HK_ERROR);
				break;

			case IDC_SAVEALL:
				if (MessageBoxW(NULL, L"Are you sure?", L"Confirm", MB_OKCANCEL | MB_ICONINFORMATION) == IDOK) {
					pushh(hDlg, ST_INP_UTH, UTH_KEY);
					pushh(hDlg, ST_INP_LTH, LTH_KEY);
					pushh(hDlg, ST_INP_PZT, PZT_KEY);
					pushh(hDlg, ST_INP_EPV, EPV_KEY);
					pushh(hDlg, ST_INP_TIME, TIME_KEY);
					pushh(hDlg, ST_INP_SQW, SQW_KEY);
					pushh(hDlg, ST_INP_SQH, SQH_KEY);
					pushh(hDlg, ST_INP_SQX1, SQX1_KEY);
					pushh(hDlg, ST_INP_SQY1, SQY1_KEY);

					pushh(hDlg, ST_INP_XX1, MSQX1_KEY);
					pushh(hDlg, ST_INP_YY1, MSQY1_KEY);
					pushh(hDlg, ST_INP_XX2, MSQX2_KEY);
					pushh(hDlg, ST_INP_YY2, MSQY2_KEY);


					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
				}
				else {
					EndDialog(hDlg, LOWORD(wParam));
					return (INT_PTR)TRUE;
				}

				break;
			case ST_BTN_UTH:
				pushh(hDlg, ST_INP_UTH, UTH_KEY);
				break;

			case ST_BTN_LTH:
				pushh(hDlg, ST_INP_LTH, LTH_KEY);
				break;

			case ST_BTN_PZT:
				pushh(hDlg, ST_INP_PZT, PZT_KEY);
				break;

			case ST_BTN_EPV:
				pushh(hDlg, ST_INP_EPV, EPV_KEY);
				break;

			case ST_BTN_TIME:
				pushh(hDlg, ST_INP_TIME, TIME_KEY);
				break;
			case ST_BTN_SQW:
				pushh(hDlg, ST_INP_SQW, SQW_KEY);
				break;

			case ST_BTN_SQH:
				pushh(hDlg, ST_INP_SQH, SQH_KEY);
				break;
			case ST_BTN_SQX1:
				pushh(hDlg, ST_INP_SQX1, SQX1_KEY);
				break;

			case ST_BTN_SQY1:
				pushh(hDlg, ST_INP_SQY1, SQY1_KEY);
				break;


			case ST_BTN_XX1:
				pushh(hDlg, ST_INP_XX1, MSQX1_KEY);
				break;

			case ST_BTN_YY1:
				pushh(hDlg, ST_INP_YY1, MSQY1_KEY);
				break;

			case ST_BTN_XX2:
				pushh(hDlg, ST_INP_XX2, MSQX2_KEY);
				break;

			case ST_BTN_YY2:
				pushh(hDlg, ST_INP_YY2, MSQY2_KEY);
				break;

			default:
				break;

			}
		}
		break;
		case WM_CLOSE:
			EndDialog(hDlg, IDCANCEL);
			return (INT_PTR)TRUE;
		}
		return (INT_PTR)FALSE;
	}

	static INT_PTR CALLBACK MyUI::CustomDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		switch (uMsg) {
		case WM_INITDIALOG:
			return TRUE; // Focus is set to the edit control on dialog initialization

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK) {
				// OK button clicked, retrieve text from the edit control
				wchar_t buffer[256];
				GetDlgItemTextW(hwndDlg, IDC_EDIT_INPUT, buffer, sizeof(buffer) / sizeof(buffer[0]));

				// Process the input as needed
				MessageBoxW(NULL, buffer, L"User Input", MB_OK | MB_ICONINFORMATION);

				// Close the dialog
				EndDialog(hwndDlg, IDOK);
			}
			else if (LOWORD(wParam) == IDCANCEL) {
				// Cancel button clicked
				EndDialog(hwndDlg, IDCANCEL);
			}
			return TRUE; // Message handled
		}
		return FALSE; // Message not handled
	}

	static INT_PTR CALLBACK MyUI::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	{
		UNREFERENCED_PARAMETER(lParam);
		switch (message)
		{
		case WM_INITDIALOG:
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


	void MyUI::mess(const wchar_t* text) {
		MessageBoxW(NULL, text, L"Message", MB_OK | MB_ICONINFORMATION);
	}
	void MyUI::messi(std::string text) {
		std::stringstream newTextStream;
		newTextStream << text;
		std::string texty = newTextStream.str();
		std::wstring newText(texty.begin(), texty.end());
		OutputDebugStringW(newText.c_str());
		MessageBoxW(NULL, newText.c_str(), L"Message", MB_OK | MB_ICONINFORMATION);
	}

	void MyUI::btnh(HWND input, HWND output, std::string key) {
		std::string ma = GetInputText(input);
		SetPreference(key, ma);
		if (HWND pztTextHandle = output)
		{
			std::stringstream newTextStream;
			newTextStream << key << ":" << ma;
			std::string text = newTextStream.str();
			std::wstring newText(text.begin(), text.end());
			SetWindowTextW(pztTextHandle, newText.c_str());
			//myUIInstance.mess(newText.c_str());
		}

	}

};


#endif // !MY_UI_H
