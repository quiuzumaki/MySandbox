#include "pch.h"
#include "Utils.h"

std::string ConvertLPCWSTRToString(LPCWSTR lpcwszStr) {
	// Determine the length of the converted string 
	int strLength = WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, nullptr, 0, nullptr, nullptr);

	// Create a std::string with the determined length 
	std::string str(strLength, 0);

	// Perform the conversion from LPCWSTR to std::string 
	WideCharToMultiByte(CP_UTF8, 0, lpcwszStr, -1, &str[0], strLength, nullptr, nullptr);

	// Return the converted std::string 
	return str;
}
