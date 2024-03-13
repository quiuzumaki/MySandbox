#include "pch.h"
#include "HookFunctions.h"

LPVOID GetFunctionPtr(LPCSTR lpModule, LPCSTR lpAPIName) {
	HMODULE hModule = GetModuleHandleA(lpModule);
	if (hModule) 
		return GetProcAddress(hModule, lpAPIName);
	return NULL;
}

BOOL InstallDetoursHook() {
	// if (lpAPIName == NULL) return FALSE;
	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)OriginalNtCreateFile, HookNtCreateFile);
	DetourAttach(&(PVOID&)OriginalNtOpenFile, HookNtOpenFile);
	DetourAttach(&(PVOID&)OriginalNtReadFile, HookNtReadFile);
	DetourAttach(&(PVOID&)OriginalNtWriteFile, HookNtWriteFile);
	DetourAttach(&(PVOID&)OriginalNtDeleteFile, HookNtDeleteFile);

	DetourTransactionCommit();

	return TRUE;
}

BOOL UninstallDetoursHook() {
	// if (lpAPIName == NULL) return FALSE;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	
	DetourAttach(&(PVOID&)OriginalNtCreateFile, HookNtCreateFile);
	DetourAttach(&(PVOID&)OriginalNtOpenFile, HookNtOpenFile);
	DetourAttach(&(PVOID&)OriginalNtReadFile, HookNtReadFile);
	DetourAttach(&(PVOID&)OriginalNtWriteFile, HookNtWriteFile);
	DetourAttach(&(PVOID&)OriginalNtDeleteFile, HookNtDeleteFile);

	DetourTransactionCommit();

	return TRUE;
}