#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h> 
#include <iostream>

#ifndef HOOK_H
#define HOOK_H

struct ModuleProcessInfo {
	HANDLE hProcess;
	MODULEINFO moduleInfo;
	ModuleProcessInfo(HANDLE hProcess, MODULEINFO moduleInfo): 
		hProcess(hProcess), moduleInfo(moduleInfo) 
	{}
	
	ModuleProcessInfo(HANDLE hProcess = NULL): hProcess(hProcess) {}
};

 // PCSTR lpModuleName[] = { "KERNEL32.dll", "USER32.dll" };
// #define SIZE_OF_MODULE std::size(lpModuleName)

extern PCSTR lpAPIKernel[];

BOOL PatchIATEntry(PBYTE pTarget, PIMAGE_IMPORT_DESCRIPTOR pModuleEntry, PCSTR pModuleName);
VOID InstallHookIAT();

PBYTE ReadModuleProcess(const ModuleProcessInfo info);
BOOL WriteModuleProcess(const ModuleProcessInfo info, LPCVOID pBuffer);

VOID InstallHookProcess(LPCVOID pBuffer);
#endif // !HOOK_H


