#pragma once

#include <iostream>
#include <algorithm>
#include <Windows.h>

#ifndef UTILS_H
#define UTILS_h

#define TYPE(id) typeid(id).name();

std::string ConvertLPCWSTRToString(LPCWSTR lpcwszStr);

std::string toLowercase(const std::string& str);

inline
std::wstring convert_str2wstr(std::string str) {
	return std::wstring(str.begin(), str.end());
}

inline
std::string convert_wstr2str(std::wstring wstr) {
	return std::string(wstr.begin(), wstr.end());
}

inline
std::string to_lower_str(std::string str) {
	std::transform(str.begin(), str.end(), str.begin(), [](CHAR c) { return std::tolower(c); });
	return str;
}

inline
std::wstring to_lower_wstr(std::wstring wstr) {
	std::transform(wstr.begin(), wstr.end(), wstr.begin(), [](WCHAR c) { return std::tolower(c); });
	return wstr;
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
