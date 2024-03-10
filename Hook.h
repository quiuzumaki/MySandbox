#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h> 
#include <iostream>
#include "HookFunctions.h"

#ifndef HOOK_H
#define HOOK_H

static PCSTR lpAPIKernel[] = { "CreateFileA", "CreateFileW", "ReadFile", "WriteFile", "DeleteFileW" };

BOOL PatchIATEntry(PBYTE pTarget, PIMAGE_IMPORT_DESCRIPTOR pModuleEntry, PCSTR pModuleName);
VOID InstallHookIAT(LPCSTR);

MODULEINFO GetModuleInfo(LPCSTR lpModuleName);

VOID InstallHookProcess(LPCVOID pBuffer);
#endif // !HOOK_H


