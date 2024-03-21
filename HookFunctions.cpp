#include "pch.h"
#include "HookFunctions.h"

ULONGLONG get_address_of_hook_function(LPCSTR lpFunctionName) {
	if (!strcmp(lpFunctionName, "DeleteFileA")) {
		return (ULONGLONG)HookDeleteFileA;
	}
	return (ULONGLONG)HookDeleteFileW;
}

BOOL patch_iat_entry(PBYTE pTarget, PIMAGE_IMPORT_DESCRIPTOR pModuleEntry) {

	PIMAGE_THUNK_DATA originalThunk = (PIMAGE_THUNK_DATA)(pTarget + pModuleEntry->OriginalFirstThunk);
	PIMAGE_THUNK_DATA firstThunk = (PIMAGE_THUNK_DATA)(pTarget + pModuleEntry->FirstThunk);

	BOOL found = FALSE;
	while (originalThunk->u1.AddressOfData != NULL) {
		PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)(pTarget + originalThunk->u1.AddressOfData);

		for (INT i = 0; i < 4; i++) {
			if (!strcmp(lpAPIKernel[i], importByName->Name)) {
				found = TRUE;
				DWORD protect = 0;
				MessageBoxA(NULL, importByName->Name, "ok", NULL);
				VirtualProtect(firstThunk, 8, PAGE_READWRITE, &protect);
				firstThunk->u1.Function = get_address_of_hook_function(lpAPIKernel[i]);
				break;
			}
		}
		originalThunk++;
		firstThunk++;
	}
	return found;
}

VOID install_hook_iat(LPCSTR lpModuleName) {
	MODULEINFO info = { };
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(lpModuleName), &info, sizeof(MODULEINFO));

	PVOID pBase = info.lpBaseOfDll;

	PBYTE imageBase = (PBYTE)pBase;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageBase;
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE || dosHeader == NULL) {
		return;
	}
	// NT Headers section
	PIMAGE_NT_HEADERS imageNTHeaders = (PIMAGE_NT_HEADERS)(imageBase + dosHeader->e_lfanew);
	if (imageNTHeaders->Signature != IMAGE_NT_SIGNATURE || imageNTHeaders == NULL) {
		return;
	}

	IMAGE_OPTIONAL_HEADER imageOptionalHeaders = imageNTHeaders->OptionalHeader;

	IMAGE_DATA_DIRECTORY imgDataDir = imageOptionalHeaders.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(imageBase + imgDataDir.VirtualAddress);

	while (importDescriptor->Name != NULL) {
		PSTR pModuleNameOfPE = PSTR(imageBase + importDescriptor->Name);
		// MessageBoxA(NULL, pModuleNameOfPE, "ok", NULL);
		if (!strcmp(pModuleNameOfPE, lpModuleName)) {
			MessageBoxA(NULL, "found module", "ok", NULL);
			patch_iat_entry(imageBase, importDescriptor);
			return;
		}
		importDescriptor++;
	}
}

BOOL InstallDetoursHook() {
	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)OriginalNtCreateFile, HookNtCreateFile);
	DetourAttach(&(PVOID&)OriginalNtOpenFile, HookNtOpenFile);
	DetourAttach(&(PVOID&)OriginalNtReadFile, HookNtReadFile);
	DetourAttach(&(PVOID&)OriginalNtWriteFile, HookNtWriteFile);
	DetourAttach(&(PVOID&)OriginalNtDeleteFile, HookNtDeleteFile);

	DetourAttach(&(PVOID&)OriginalDeleteFileW, HookDeleteFileW);
	DetourAttach(&(PVOID&)OriginalDeleteFileA, HookDeleteFileA);

	DetourTransactionCommit();

	return TRUE;
}

BOOL UninstallDetoursHook() {

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	
	DetourAttach(&(PVOID&)OriginalNtCreateFile, HookNtCreateFile);
	DetourAttach(&(PVOID&)OriginalNtOpenFile, HookNtOpenFile);
	DetourAttach(&(PVOID&)OriginalNtReadFile, HookNtReadFile);
	DetourAttach(&(PVOID&)OriginalNtWriteFile, HookNtWriteFile);
	DetourAttach(&(PVOID&)OriginalNtDeleteFile, HookNtDeleteFile);

	DetourAttach(&(PVOID&)OriginalDeleteFileW, HookDeleteFileW);
	DetourAttach(&(PVOID&)OriginalDeleteFileA, HookDeleteFileA);

	DetourTransactionCommit();

	return TRUE;
}