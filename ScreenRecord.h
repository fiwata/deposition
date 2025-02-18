#ifndef SCREEN_RECORD_H
#define SCREEN_RECORD_H
#include <Windows.h>
#include <string>
#include "Resource.h"
#include "preferencemanager.h"

void CaptureAndSaveScreenshot()
{
	std::string folderName = getprefString(CURRENT_VIDEO_FOLDER);
	std::string key = folderName + "\\" + getprefString(CURRENT_FILENAME_KEY);
	std::wstring fileName(key.begin(), key.end());

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
	SelectObject(hdcMem, hBitmap);

	BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);

	BITMAPINFO bmpInfo;
	ZeroMemory(&bmpInfo, sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = screenWidth;
	bmpInfo.bmiHeader.biHeight = -screenHeight;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	DWORD dataSize;
	GetDIBits(hdcMem, hBitmap, 0, screenHeight, nullptr, &bmpInfo, DIB_RGB_COLORS);
	dataSize = bmpInfo.bmiHeader.biSizeImage;

	BYTE* buffer = new BYTE[dataSize];
	GetDIBits(hdcMem, hBitmap, 0, screenHeight, buffer, &bmpInfo, DIB_RGB_COLORS);

	// Add BMP file header
	BITMAPFILEHEADER bmpFileHeader;
	bmpFileHeader.bfType = 0x4D42;
	bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + bmpInfo.bmiHeader.biSize + dataSize;
	bmpFileHeader.bfReserved1 = 0;
	bmpFileHeader.bfReserved2 = 0;
	bmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + bmpInfo.bmiHeader.biSize;

	std::wstring filename_ = L"";
	filename_ += fileName;
	filename_ += L".BMP";

	HANDLE hFile = CreateFileW(filename_.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD bytesWritten;
		WriteFile(hFile, &bmpFileHeader, sizeof(BITMAPFILEHEADER), &bytesWritten, nullptr);
		WriteFile(hFile, &bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), &bytesWritten, nullptr);
		WriteFile(hFile, buffer, dataSize, &bytesWritten, nullptr);
		CloseHandle(hFile);
	}

	delete[] buffer;
	DeleteObject(hBitmap);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);
}


#endif