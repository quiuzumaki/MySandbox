#pragma once

#include <iostream>
#include <algorithm>
#include <Windows.h>
#include <codecvt>

#ifndef UTILS_H
#define UTILS_h

#define TYPE(id) typeid(id).name();

std::string ConvertLPCWSTRToString(LPCWSTR lpcwszStr);

std::string toLowercase(const std::string& str);

inline
std::string to_lower_str(std::string str) {
	std::string buffer;
	std::transform(str.begin(), str.end(), buffer.begin(), [](CHAR c) { return std::tolower(c); });
	return buffer;
}

inline
std::wstring to_lower_wstr(std::wstring wstr) {
	std::wstring buffer;
	std::transform(wstr.begin(), wstr.end(), buffer.begin(), [](WCHAR c) { return std::tolower(c); });
	return buffer;
}

inline
std::string convert_lpcstr2str(LPCSTR str) {
	return std::string(str);
}

inline
std::wstring convert_lpcwstr2wstr(LPCWSTR wstr) {
	return std::wstring(wstr);
}

#endif // !UTILS_H
