#pragma once

#include <windows.h>
#include <CommDlg.h>
#include <vector>

using namespace std;

std::string getAppPath()
{
	char appPath[MAX_PATH] = "";

	// fetch the path of the executable
	::GetModuleFileNameA(0, appPath, sizeof(appPath) - 1);

	// get a pointer to the last occurrence of the windows path separator
	char *appDir = strrchr(appPath, '\\');
	if (appDir) {
		++appDir;

		// always expect the unexpected - this shouldn't be null but one never knows
		if (appDir) {
			// null terminate the string
			*appDir = 0;
		}
	}

	return std::string(appPath);
}


std::string getOpenFilePath(HWND hWnd, const std::string &initialPath, vector<std::string> extensions)
{
	OPENFILENAMEA ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name

							// Initialize OPENFILENAME
	::ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;

	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	if (extensions.empty()) {
		ofn.lpstrFilter = "All\0*.*\0";
	}
	else {
		char extensionStr[10000];
		size_t offset = 0;

		strcpy(extensionStr, "shadertoy pixel shader(.ps)");
		offset += strlen(extensionStr) + 1;
		for (vector<string>::const_iterator strIt = extensions.begin(); strIt != extensions.end(); ++strIt) {
			strcpy(extensionStr + offset, "*.");
			offset += 2;
			strcpy(extensionStr + offset, strIt->c_str());
			offset += strIt->length();
			// append a semicolon to all but the last extensions
			if (strIt + 1 != extensions.end()) {
				extensionStr[offset] = ';';
				offset += 1;
			}
			else {
				extensionStr[offset] = 0;
				offset += 1;
			}
		}

		extensionStr[offset] = 0;
		ofn.lpstrFilter = extensionStr;
	}
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	if (initialPath.empty())
		ofn.lpstrInitialDir = NULL;
	else {
		char initialPathStr[MAX_PATH];
		strcpy(initialPathStr, initialPath.c_str());
		ofn.lpstrInitialDir = initialPathStr;
	}
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box.
	if (::GetOpenFileNameA(&ofn) == TRUE) {
		return string(ofn.lpstrFile);
	}
	else
		return string();
}