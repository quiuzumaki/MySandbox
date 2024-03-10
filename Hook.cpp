#include "pch.h"
#include "Hook.h"

BOOL PatchIATEntry(PBYTE pTarget, PIMAGE_IMPORT_DESCRIPTOR pModuleEntry, PCSTR pModuleName) {

	PIMAGE_THUNK_DATA originalThunk = (PIMAGE_THUNK_DATA)(pTarget + pModuleEntry->OriginalFirstThunk);
	PIMAGE_THUNK_DATA firstThunk = (PIMAGE_THUNK_DATA)(pTarget + pModuleEntry->FirstThunk);

	BOOL found = FALSE;
	while (originalThunk->u1.AddressOfData != NULL) {
		PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)(pTarget + originalThunk->u1.AddressOfData);

		for (INT i = 0; i < 4; i++) {
			if (!strncmp(lpAPIKernel[i], importByName->Name, strlen(lpAPIKernel[i]))) {
				found = TRUE;
				DWORD protect = 0;
				VirtualProtect(firstThunk, 8, PAGE_READWRITE, &protect);
				MessageBox(GetActiveWindow(), lpAPIKernel[i], "Patch IAT", MB_OK);
				firstThunk->u1.Function = GetAddressOfHook_KERNEL32(lpAPIKernel[i]);
				break;
			}
		}

		originalThunk++;
		firstThunk++;
	}
	return found;
}

VOID InstallHookIAT(LPCSTR lpModuleName) {
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

		if (!strcmp(pModuleNameOfPE, "KERNEL32.dll")) {
			MessageBox(GetActiveWindow(), pModuleNameOfPE, "Install Hook IAT", MB_OK);
			PatchIATEntry(imageBase, importDescriptor, "KERNEL32.dll");
		}
		importDescriptor++;
	}

}

VOID InstallHookProcess(LPCVOID pTarget) {
	PBYTE imageBase = (PBYTE)pTarget;

	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)imageBase;
	
	if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE || dosHeader == NULL) {
		std::runtime_error("DOS Headers is invalid: " + std::to_string(GetLastError()));
	}
	
	// NT Headers section
	PIMAGE_NT_HEADERS imageNTHeaders = (PIMAGE_NT_HEADERS)(imageBase + dosHeader->e_lfanew);
	
	if (imageNTHeaders->Signature != IMAGE_NT_SIGNATURE || imageNTHeaders == NULL) {
		std::runtime_error("NT Headers is invalid: " + std::to_string(GetLastError()));
	}
	
	IMAGE_OPTIONAL_HEADER imageOptionalHeaders = imageNTHeaders->OptionalHeader;
	IMAGE_DATA_DIRECTORY imgDataDir = imageOptionalHeaders.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(imageBase + imgDataDir.VirtualAddress);

	while (importDescriptor->Name != NULL) {
		PSTR pModuleNameOfPE = PSTR(imageBase + importDescriptor->Name);
		if (!strcmp(pModuleNameOfPE, "KERNEL32.dll")) {
			PatchIATEntry(imageBase, importDescriptor, "KERNEL32.dll");
		}

		importDescriptor++;
	}
}