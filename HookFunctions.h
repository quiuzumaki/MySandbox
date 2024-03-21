#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h> 
#include <detours.h>

#include "File.h"
#include "Logs.h"

#ifndef HOOKFUNCTIONS_H
#define HOOKFUNCTIONS_H

inline
LPVOID GetFunctionPtr(LPCSTR lpModule, LPCSTR lpAPIName) {
	HMODULE hModule = GetModuleHandleA(lpModule);
	if (hModule)
		return GetProcAddress(hModule, lpAPIName);
	return NULL;
}

static PCSTR lpAPIKernel[] = { "DeleteFileA", "DeleteFileW" };

ULONGLONG get_address_of_hook_function(LPCSTR);
BOOL patch_iat_entry(PBYTE pTarget, PIMAGE_IMPORT_DESCRIPTOR pModuleEntry);
VOID install_hook_iat(LPCSTR lpModuleName);

BOOL InstallDetoursHook();
BOOL UninstallDetoursHook();

#endif // !HOOKFUNCTIONS_H
