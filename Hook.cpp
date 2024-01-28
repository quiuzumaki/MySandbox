#include "pch.h"
#include "Hook.h"
#include "HookFunctions.h"

PCSTR lpAPIKernel[] = { "CreateFileA", "CreateFileW", "ReadFile", "ReadFileEx", "DeleteFileA"};

BOOL PatchIATEntry(PBYTE pTarget, PIMAGE_IMPORT_DESCRIPTOR pModuleEntry, PCSTR pModuleName) {

	PIMAGE_THUNK_DATA originalThunk = (PIMAGE_THUNK_DATA)(pTarget + pModuleEntry->OriginalFirstThunk);
	PIMAGE_THUNK_DATA firstThunk = (PIMAGE_THUNK_DATA)(pTarget + pModuleEntry->FirstThunk);

	BOOL found = FALSE;
	while (originalThunk->u1.AddressOfData != NULL) {
		PIMAGE_IMPORT_BY_NAME importByName = (PIMAGE_IMPORT_BY_NAME)(pTarget + originalThunk->u1.AddressOfData);

		for (INT i = 0; i < 4; i++) {
			if (!strncmp(lpAPIKernel[i], importByName->Name, strlen(lpAPIKernel[i]))) {
				found = TRUE;
				MessageBox(NULL, "Install Hook SuccessFull", "CreateFile", NULL);
				DWORD protect = 0;
				VirtualProtect(firstThunk, 8, PAGE_READWRITE, &protect);
				firstThunk->u1.Function = GetAddressOfHook_KERNEL32(lpAPIKernel[i]);
				break;
			}
		}

		originalThunk++;
		firstThunk++;
	}
	return found;
}

VOID InstallHookIAT() {
	PVOID pBase = GetModuleHandleA(NULL);

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
			PatchIATEntry(imageBase, importDescriptor, "KERNEL32.dll");
		}

		importDescriptor++;
	}

}

PBYTE ReadModuleProcess(const ModuleProcessInfo info) {
	MODULEINFO moduleInfo = info.moduleInfo;
	PBYTE buffer = (PBYTE)malloc(moduleInfo.SizeOfImage + 1);
	// PBYTE buffer = (PBYTE)VirtualAllocEx(GetModuleHandleA(0), NULL, info.moduleInfo.SizeOfImage + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	SIZE_T NumberOfBytesToRead = moduleInfo.SizeOfImage;
	SIZE_T NumberOfBytesActuallyRead;
	
	try {
		ReadProcessMemory(info.hProcess, (LPVOID)moduleInfo.lpBaseOfDll, buffer, NumberOfBytesToRead, &NumberOfBytesActuallyRead);
	}
	catch (const std::exception& e) {
		throw std::runtime_error("cannot write all byte in process" + std::to_string(GetLastError()));
	}

	if (NumberOfBytesActuallyRead != NumberOfBytesToRead) {
		throw std::runtime_error("cannot read all byte in process" + std::to_string(GetLastError()));
	}

	return buffer;
}

BOOL WriteModuleProcess(const ModuleProcessInfo info, LPCVOID pBuffer) {
	MODULEINFO moduleInfo = info.moduleInfo;
	SIZE_T NumberOfBytesToWrite = moduleInfo.SizeOfImage;
	SIZE_T NumberOfBytesActuallyWrite;

	try {
		
		WriteProcessMemory(info.hProcess, moduleInfo.lpBaseOfDll, pBuffer, NumberOfBytesToWrite, &NumberOfBytesActuallyWrite);

	} catch (const std::exception& e) {
		throw std::runtime_error("WriteProcessMemory is error!!" + std::to_string(GetLastError()));
	}
	

	if (NumberOfBytesActuallyWrite != NumberOfBytesToWrite) {
		throw std::runtime_error("cannot write all byte in process" + std::to_string(GetLastError()));
	}

	return TRUE;
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