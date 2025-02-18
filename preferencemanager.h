#ifndef PREFERENCE_MANAGER_H
#define PREFERENCE_MANAGER_H

#include <windows.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "Resource.h"
#include <map>
#include <iostream>
#include <Shlobj.h>
#include <Shlwapi.h>

std::string filename;
std::string getprefString(const std::string& key);
std::string getprfStr(const std::string& key);

struct Coordinate {
    double x, y, z;
    std::string toString() const {
        std::stringstream ss;
        ss << x << ", " << y << "," << z << "\n";
        return ss.str();
    }
};

Coordinate* readCSV(const std::string& filename, size_t& count) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file";
        return nullptr;
    }

    // First, count the number of lines
    count = 0;
    std::string line;
    while (std::getline(file, line)) {
        ++count;
    }

    // Allocate memory for the coordinates
    Coordinate* coordinates = new Coordinate[count];

    // Reset the file stream and read the lines into the array
    file.clear();
    file.seekg(0, std::ios::beg);
    size_t index = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        coordinates[index].x = std::stod(token);
        std::getline(ss, token, ',');
        coordinates[index].y = std::stod(token);
        std::getline(ss, token, ',');
        coordinates[index].z = std::stod(token);

        ++index;
    }

    file.close();
    return coordinates;
}


void Logger(const std::string& message, const std::string& level);
// Function to get the current time as a string
std::string GetCurrentT() {
    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}

// Logger function
void Logger(const std::string& message,const std::string& level) {
    std::ofstream logFile;
    logFile.open(getprfStr(CURRENT_VIDEO_FOLDER) + "\\" + "log.log", std::ios_base::app); // Open the log file in append mode
    if (logFile.is_open()) {
        logFile << GetCurrentT() << " - "<<level << " - " << message << std::endl;
        logFile.close();
    }
}

bool isFilexists(std::string filepath) {
    std::ifstream file(filepath);
    if (file) {
        return true;
    }
    else {
        return false;
    }
}
// Function to save preferences to the registry
bool SetPreference(const std::string& key, const std::string& value) {
    //Logger("SetPreference key:- "+key + " value:-" + value);
    std::wstring wideKey(key.begin(), key.end());
    std::wstring wideValue(value.begin(), value.end());
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\YourAppName", 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        if (RegSetValueExW(hKey, wideKey.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(wideValue.c_str()), static_cast<DWORD>(sizeof(wchar_t) * (wideValue.length() + 1))) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}
bool SetPreferenceW(const std::string& key, const wchar_t* text) {
    std::wstring wideKey(key.begin(), key.end());
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\YourAppName", 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        if (RegSetValueExW(hKey, wideKey.c_str(), 0, REG_SZ, reinterpret_cast<const BYTE*>(text), static_cast<DWORD>((wcslen(text) + 1) * sizeof(wchar_t))) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}
std::string getprefString(const std::string& key) {
    std::wstring wideKey(key.begin(), key.end());
    std::wstring value;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\YourAppName", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD bufferSize = 1024; // Adjust this size based on your needs
        wchar_t buffer[1024];
        if (RegQueryValueExW(hKey, wideKey.c_str(), nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) {
            value = buffer;
        }
        RegCloseKey(hKey);
    }
    std::string resu = std::string(value.begin(), value.end());
    //Logger("getprefString key:- " + key + " value:-" + resu);
    return resu;
}

std::string getprfStr(const std::string& key) {
    std::wstring wideKey(key.begin(), key.end());
    std::wstring value;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\YourAppName", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD bufferSize = 1024; // Adjust this size based on your needs
        wchar_t buffer[1024];
        if (RegQueryValueExW(hKey, wideKey.c_str(), nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer), &bufferSize) == ERROR_SUCCESS) {
            value = buffer;
        }
        RegCloseKey(hKey);
    }
    return std::string(value.begin(), value.end());
}

// Function to load preferences from the registry
DWORD LoadPreferences(const wchar_t* keyName, const wchar_t* valueName, DWORD defaultValue) {
    DWORD value = defaultValue;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, keyName, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dataSize = sizeof(DWORD);
        RegQueryValueExW(hKey, valueName, NULL, NULL, reinterpret_cast<BYTE*>(&value), &dataSize);
        RegCloseKey(hKey);
    }
    return value;
}


bool CheckAndCreateFolder(std::string folder, std::string subfolder) {

    int size = MultiByteToWideChar(CP_UTF8, 0, folder.c_str(), -1, nullptr, 0);
    int sizer = MultiByteToWideChar(CP_UTF8, 0, subfolder.c_str(), -1, nullptr, 0);

    wchar_t* folderName = new wchar_t[size];
    wchar_t* subfolderName = new wchar_t[sizer];

    MultiByteToWideChar(CP_UTF8, 0, folder.c_str(), -1, folderName, size);
    MultiByteToWideChar(CP_UTF8, 0, subfolder.c_str(), -1, subfolderName, sizer);

    wchar_t desktopPath[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktopPath) != S_OK) {
        return false;
    }

    wchar_t fullPath[MAX_PATH];
    if (PathCombineW(fullPath, desktopPath, folderName) == NULL) {
        return false;
    }

    CreateDirectoryW(fullPath, NULL);

    wchar_t subfolderPath[MAX_PATH];
    if (PathCombineW(subfolderPath, fullPath, subfolderName) == NULL) {
        return false;
    }

    CreateDirectoryW(subfolderPath, NULL);

    SetPreferenceW(CURRENT_FOLDER, subfolderPath);
    delete[] subfolderName;
    delete[] folderName;

    return true;
}

bool CheckAndCreateVideoFolder(std::string folder, std::string subfolder) {

    int size = MultiByteToWideChar(CP_UTF8, 0, folder.c_str(), -1, nullptr, 0);
    int sizer = MultiByteToWideChar(CP_UTF8, 0, subfolder.c_str(), -1, nullptr, 0);

    wchar_t* folderName = new wchar_t[size];
    wchar_t* subfolderName = new wchar_t[sizer];

    MultiByteToWideChar(CP_UTF8, 0, folder.c_str(), -1, folderName, size);
    MultiByteToWideChar(CP_UTF8, 0, subfolder.c_str(), -1, subfolderName, sizer);

    wchar_t desktopPath[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_MYVIDEO, NULL, 0, desktopPath) != S_OK) {
        return false;
    }

    wchar_t fullPath[MAX_PATH];
    if (PathCombineW(fullPath, desktopPath, folderName) == NULL) {
        return false;
    }

    CreateDirectoryW(fullPath, NULL);

    wchar_t subfolderPath[MAX_PATH];
    if (PathCombineW(subfolderPath, fullPath, subfolderName) == NULL) {
        return false;
    }

    CreateDirectoryW(subfolderPath, NULL);

    SetPreferenceW(CURRENT_VIDEO_FOLDER, subfolderPath);
    delete[] subfolderName;
    delete[] folderName;

    return true;
}
void debug(std::string val) {
    std::wstring wideVa(val.begin(), val.end());
    OutputDebugStringW(L"\n");
    OutputDebugStringW(wideVa.c_str());
    OutputDebugStringW(L"\n");
}
class SchmittTrigger {
public:
    SchmittTrigger(double upperThreshold, double lowerThreshold)
        : upperThreshold_(upperThreshold), lowerThreshold_(lowerThreshold), output_(false) {
    }
    bool processInput(double inputValue) {
        if (inputValue >= upperThreshold_) {
            output_ = true;
        }
        else if (inputValue <= lowerThreshold_) {
            output_ = false;
        }
        return output_;
    }
    bool getOutput() const {
        return output_;
    }

private:
    double upperThreshold_;
    double lowerThreshold_;
    bool output_;
};

#endif // PREFERENCE_MANAGER_H
