#include "framework.h"
#include <atomic>
#include <algorithm>
#include "ScreenRecord.h"
#include "resource.h"
#include "framework.h"
#include <atomic>
#include "exportCSV.h"
#include <string>
#include <nidaqmx.h>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include "preferencemanager.h"
#include "MyUi.h"
#include "ExportCsv.h"
#include <memory>
#include "constants.h"
#include "cdialog.h"
#include "cam.h"
#include "render.h"
#include <commdlg.h>
#include "DaqSignal.h"

#define MAX_LOADSTRING 100

HWND  hFrame;
std::deque<double> brightData;
TaskHandle lserOn = nullptr;
TaskHandle lserOff = nullptr;
MyUI myUIInstance;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING], szWindowClass[MAX_LOADSTRING];
std::mutex mtx;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
std::atomic<double> pztVolt(0.0);
cv::VideoWriter vrr;

Cam camera;

void onCameraOn() {
	camera.start();
	while (true) {
		cv::Mat frame = camera.getFrame();
		if (frame.empty()) break;//break mean stop
		FrameRenderer renderer(myUIInstance.getCamCam());
		renderer.Render(frame);
		if (cv::waitKey(1) == 'q') break;
	}
}

void onCameraOff() {
	camera.stop();
}


void yymmdd_hhmmss(char* formattedDateTime, size_t bufferSize) {
	SYSTEMTIME st;
	GetLocalTime(&st);
	_snprintf_s(formattedDateTime, bufferSize, _TRUNCATE, "%02d%02d%02d_%02d%02d%02d",
		st.wYear % 100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));
    MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;
   const int x = std::round(0.50 * GetSystemMetrics(SM_CXSCREEN));
   const int y = std::round(0.05 * GetSystemMetrics(SM_CYSCREEN));
   const int w = std::round(0.90 * GetSystemMetrics(SM_CXSCREEN));
   const int h = std::round(0.90 * GetSystemMetrics(SM_CYSCREEN));

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, 0,y, w, h, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}
void pushh(int controlId,std::string key) {
	HWND hInput = GetDlgItem(hFrame, controlId);

	// Get the length of the text
	int textLength = GetWindowTextLengthW(hInput) + 1;

	// Allocate a buffer to hold the text
	wchar_t* buffer = new wchar_t[textLength];

	// Get the text from the input control
	GetWindowTextW(hInput, buffer, textLength);

	// Convert to std::string
	std::string result(buffer, buffer + textLength - 1); // Exclude null terminator from the length
	// Clean up
	delete[] buffer;

	SetPreference(key, result);
}

void file_csv_explorer(HWND hwnd,int inputID,const std::string &key) {
	OPENFILENAMEW ofn;       // common dialog box structure
	wchar_t szFile[260];     // buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;

	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = L'\0'; // Wide-character null terminator
	ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]); // Size in WCHARs
	ofn.lpstrFilter = L"All Files\0*.*\0"; // Wide-character string
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box
	if (GetOpenFileNameW(&ofn) == TRUE) {
		SetWindowTextW(GetDlgItem(hwnd, inputID), ofn.lpstrFile);  // Set the selected file path to the input box
		SetPreferenceW(key, ofn.lpstrFile);
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_FRAMERATE_UPDATED:
	{
		myUIInstance.messi("rrrr");
	}
	break;

	case WM_CREATE:
	{
		hFrame = myUIInstance.mainUi(hWnd);
		SetWindowLongPtr(hFrame, GWLP_WNDPROC, (LONG_PTR)WndProc);
	}
	break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {

			case ID_BTN_CAMERA_ON:
				if (SetPreference(CURRENT_FILENAME_KEY, myUIInstance.yymmdd_hhmmss())) {
					SetPreference(DEPOSITION_ON_OFF, "OFF");
					SetPreference(CameraONoFF, "ON");
					onCameraOn();
					Logger("Camera on..", "INFO");
				}
				break;
			case ID_BTN_CAMERA_OFF:
				if (SetPreference(CURRENT_FILENAME_KEY, myUIInstance.yymmdd_hhmmss())) {
					SetPreference(CameraONoFF, "OFF");
					onCameraOff();
					Logger("Camera off..", "INFO");
				}
				break;

			case ID_BTN_CAMERA_CAMAC: {
				 for (int i = 0; i < 30; ++i) {  // Checking up to 10 camera indexes
						cv::VideoCapture cap(i);
						if (cap.isOpened()) {
							std::string ff = std::to_string(i);
							myUIInstance.messi(ff);
							cap.release();  // Release the VideoCapture
						} else {
							std::cout << "Camera index " << i << " is not available." << std::endl;
						}
					}
				break;
			}
			case ID_BTN_CAMERA_RE: {
				
				break;
			}

			case ID_BTN_LASER_ON:
			{
				(getprefString(LASER_TOGGLE) == "on") ? SetPreference(LASER_TOGGLE, "off") : SetPreference(LASER_TOGGLE, "on");
				if (getprefString(LASER_TOGGLE) == "on") {
					Digitaloutput(1);
					Logger("laser btn on pass 1","INFO");
				}
				else {
					Digitaloutput(0);
					Logger("laser btn on pass 0","INFO");

				}
				
			}
			break;
			case ID_BTN_EPDV0:
			{
				DDaq(NI_VOLTS_0, NI_DEVICESk(), NI_PORTS);
				Logger("call :-DDaq(NI_VOLTS_0, NI_DEVICESk(), NI_PORTS) by press ID_BTN_EPDV0","INFO");


			}
				break;
				
			case ID_BTN_PZTV0:
				{
					DDaq({0}, {"Dev2"}, {1});
				}
				break;
			case XYZ_CSV_BROWSE:
			{
				file_csv_explorer(hWnd, XYZ_CSV_INPUT, XYZ_CSV_FILENAME);
				size_t count = 0;
				Coordinate* coordinates = readCSV(filename, count);
			}
			break;
			case ID_BTN_DEPOSITION_ON:
			{
				if (SetPreference(CURRENT_FILENAME_KEY, myUIInstance.yymmdd_hhmmss())) {
					SetPreference(DEPOSITION_ON_OFF, "ON");
				}				
			}
			break;
			case ID_BTN_DEPOSITION_OFF:
			{
				SetPreference(DEPOSITION_ON_OFF, "OFF");
				Logger("Deposition off button pressed..", "INFO");

			}
			break;
			case BTN_UTH:
			{
				pushh(INPUT_UTH, UTH_KEY);
			}
			break;
			case BTN_LTH:
			{
				pushh(INPUT_LTH, LTH_KEY);
			}
			break;
			case BTN_PZT:
			{
				pushh(INPUT_PZT, PZT_KEY);
			}
			break;
			case BTN_SQH:
			{
				pushh(INPUT_SQH, SQH_KEY);
			}
			break;
			case BTN_SQW:
			{
				pushh(INPUT_SQW, SQW_KEY);
			}
			break;
			case BTN_SQX:
			{
				pushh(INPUT_SQX, SQX1_KEY);
			}
			break;
			case BTN_SQY:
			{
				pushh(INPUT_SQY, SQY1_KEY);
			}
			break;
			case BTN_MSQX1:
			{
				pushh(INPUT_MSQX1, MSQX1_KEY);
			}
			break;
			case BTN_MSQY1:
			{
				pushh(INPUT_MSQY1, MSQY1_KEY);
			}
			break;
			case BTN_MSQX2:
			{
				pushh(INPUT_MSQX2, MSQX2_KEY);
			}
			break;
			case BTN_MSQY2:
			{
				pushh(INPUT_MSQY2, MSQY2_KEY);
			}
			break;
			case BTN_TIME:
			{
				pushh(INPUT_TIME, TIME_KEY);
			}
			break;
			
			case CAM_INDEX_BTN:
			{
				if (getprefString(CAM_1_0_KEY) == "on") {
					file_csv_explorer(hWnd, CAM_INDEX_INPUT, CameraIndex);
				}
				else {
					pushh(CAM_INDEX_INPUT, CameraIndex);
				}
			}
			break;
			case TGL_BTN_GRAPH:
			{
				(getprefString(AUTOGRAPH_KEY) == "on") ? SetPreference(AUTOGRAPH_KEY, "off") : SetPreference(AUTOGRAPH_KEY, "on");
			}
			break;
			case TGL_BTN_RECORD:
			{
				(getprefString(AUTORECORD_KEY) == "on") ? SetPreference(AUTORECORD_KEY, "off") : SetPreference(AUTORECORD_KEY, "on");
			}
			break;
			case TGL_BTN_SDORDIFF:
			{
				(getprefString(ADORDIFF_KEY) == "on") ? SetPreference(ADORDIFF_KEY, "off") : SetPreference(ADORDIFF_KEY, "on");
			}
			break;
			case TGL_BTN_RBOX:
			{
				(getprefString(EBOXONOFF_KEY) == "on") ? SetPreference(EBOXONOFF_KEY, "off") : SetPreference(EBOXONOFF_KEY, "on");
			}
			break;

			case ID_CAMERA_OPTION:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_CAMERA_OPTIONS), hWnd, myUIInstance.CameraOptions);
				break;
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, myUIInstance.About);
				break;
			case ID_M_BTN_U:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_C_D_BOX), hWnd, Cdialog);
				break;
			case IDM_EXIT:
			{
				SetPreference(CameraONoFF, "OFF");
				if (DialogBox(hInst, MAKEINTRESOURCE(IDD_C_D_BOX), hWnd, Cdialog) == IDOK) {
					DestroyWindow(hWnd);
				}
			}
			break;
			case MENU_SUB_VIRTUAL:
			{
				SetPreference(MENU_SUB_VIR_REA, "ON");
				break;
			}
			case MENU_SUB_REAL:
			{
				SetPreference(MENU_SUB_VIR_REA, "OFF");
				break;
			}
			case STAG_TOGGLE:
			{
				(getprefString(STAG_TOGGLE_KEY) == "on") ? SetPreference(STAG_TOGGLE_KEY, "off") : SetPreference(STAG_TOGGLE_KEY, "on");
				break;
			}
			case THRESHOLD_TOGGLE:
			{
				(getprefString(THRESHOLD_TOGGLE_KEY) == "on") ? SetPreference(THRESHOLD_TOGGLE_KEY, "off") : SetPreference(THRESHOLD_TOGGLE_KEY, "on");
				break;
			}
			case STAG_X_TOGGLE:
			{
				(getprefString(STAG_X_TOGGLE_KEY) == "on") ? SetPreference(STAG_X_TOGGLE_KEY, "off") : SetPreference(STAG_X_TOGGLE_KEY, "on");
				break;
			}
			case STAG_Y_TOGGLE:
			{
				(getprefString(STAG_Y_TOGGLE_KEY) == "on") ? SetPreference(STAG_Y_TOGGLE_KEY, "off") : SetPreference(STAG_Y_TOGGLE_KEY, "on");
				break;
			}
			case STAG_Z_TOGGLE:
			{
				(getprefString(STAG_Z_TOGGLE_KEY) == "on") ? SetPreference(STAG_Z_TOGGLE_KEY, "off") : SetPreference(STAG_Z_TOGGLE_KEY, "on");
				break;
			}
			
			case CAM_1_0_:
			{
				(getprefString(CAM_1_0_KEY) == "on") ? SetPreference(CAM_1_0_KEY, "off") : SetPreference(CAM_1_0_KEY, "on");
				break;
			}
			case TAKE_SCREENSHOT:
			{
				CaptureAndSaveScreenshot();
			}

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;

	case WM_CLOSE:
	{
		SetPreference(CameraONoFF, "OFF");
		onCameraOff();
		if (getprefString(CameraONoFF) == "OFF") {
			if (DialogBox(hInst, MAKEINTRESOURCE(IDD_C_D_BOX), hWnd, Cdialog) == IDOK) {
				DestroyWindow(hWnd);
			}

		}
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_MOUSEMOVE:
	{
		int x = (LOWORD(lParam)/3)+0;
		int y = (HIWORD(lParam))+37;
		SetPreference(MOUSE_X, std::to_string(x));
		SetPreference(MOUSE_Y, std::to_string(y));
	}
	break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

