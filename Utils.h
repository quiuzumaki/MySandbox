#pragma once

#include <iostream>
#include <Windows.h>

#ifndef UTILS_H
#define UTILS_h

#define TYPE(id) typeid(id).name();

std::string ConvertLPCWSTRToString(LPCWSTR lpcwszStr);
std::string toLowercase(const std::string& str);

#endif // !UTILS_H
