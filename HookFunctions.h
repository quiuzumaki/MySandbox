#pragma once

#include <Windows.h>
#include "File.h"
#include <detours.h>

#ifndef HOOKFUNCTIONS_H
#define HOOKFUNCTIONS_H

#if _WIN64
typedef ULONGLONG ADDRESS_SIZE;
#else
typedef DWORD ADDRESS_SIZE;
#endif

ADDRESS_SIZE GetAddressOfHook_KERNEL32(PCSTR lpAPIName);
ADDRESS_SIZE GetAddressOfHook_FILE(PCSTR lpAPIName);
ADDRESS_SIZE GetAddressOfHook_REGISTRY(PCSTR lpAPIName);

LPVOID GetFunctionPtr(LPCSTR lpModule, LPCSTR lpAPIName); 

BOOL InstallDetoursHook();
BOOL UninstallDetoursHook();

BOOL Attack();

#endif // !HOOKFUNCTIONS_H
