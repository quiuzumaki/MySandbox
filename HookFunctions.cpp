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
	else if (!strcmp(lpAPIName, "ReadFileEx")) {
		return (ADDRESS_SIZE)HookReadFile;
	}
	else if (!strcmp(lpAPIName, "WriteFile")) {
		return (ADDRESS_SIZE)HookWriteFile;
	}
	else if (!strcmp(lpAPIName, "WriteFileEx")) {
		return (ADDRESS_SIZE)HookWriteFileEx;
	}
	else if (!strcmp(lpAPIName, "DeleteFileA")) {
		return (ADDRESS_SIZE)HookDeleteFileA;
	}
	return 0;
}

ADDRESS_SIZE GetAddressOfHook_REGISTRY(PCSTR lpAPIName) {
	return 0;
}

