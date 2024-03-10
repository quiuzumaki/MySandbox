#include "pch.h"
#include "HookFunctions.h"

ADDRESS_SIZE GetAddressOfHook_KERNEL32(PCSTR lpAPIName) {
	if (strstr(lpAPIName, "File") != NULL) {
		return GetAddressOfHook_FILE(lpAPIName);
	}
	// return GetAddressOfHook_REGISTRY(lpAPIName);
	return NULL;
}

ADDRESS_SIZE GetAddressOfHook_FILE(PCSTR lpAPIName) {
	if (!strcmp(lpAPIName, "CreateFileA")) {
		return (ADDRESS_SIZE)HookCreateFileA;
	}
	else if (!strcmp(lpAPIName, "CreateFileW")) {
		return (ADDRESS_SIZE)HookCreateFileW;
	}
	else if (!strcmp(lpAPIName, "ReadFile")) {
		return (ADDRESS_SIZE)HookReadFile;
	}
	else if (!strcmp(lpAPIName, "WriteFile")) {
		return (ADDRESS_SIZE)HookWriteFile;
	}
	else if (!strcmp(lpAPIName, "DeleteFileW")) {
		return (ADDRESS_SIZE)HookDeleteFileW;
	}
	return 0;
}

ADDRESS_SIZE GetAddressOfHook_REGISTRY(PCSTR lpAPIName) {
	return 0;
}

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