#pragma once

#include <Windows.h>
#include "File.h"

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

#endif // !HOOKFUNCTIONS_H
