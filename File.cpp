#pragma once

#include "pch.h"
#include "File.h"

inline BOOL is_belong_to(fs::path path) {
	char env_path[MAX_PATH];
	std::string relative_path = path.relative_path().string();

	for (int i = 0; i < sizeof(lstDIRID) / sizeof(int); i++) {
		if (SUCCEEDED(SHGetFolderPathA(NULL, lstDIRID[i], NULL, 0, env_path))) {
			if (relative_path.find(env_path, 0) != std::string::npos)
				return TRUE;
		}
	}
	
	return FALSE;
}

BOOL path_is_allowed(std::string pathName) {

	fs::path path(pathName);
	if (
		is_belong_to(path)
	) {
		return TRUE;
	}

	if (check_extensions(path)) {
		return TRUE;
	}

	return FALSE;
}

inline BOOL check_extensions(fs::path pathExtension) {
	
	if (!pathExtension.has_extension()) {
		return FALSE;
	} 
	std::string extension = pathExtension.extension().string();
	for (int i = 0; i < sizeof(lstExtentions) / sizeof(std::string); i++) {
		
		if (!strcmp(extension.c_str(), lstExtentions[i].c_str())) {
			return TRUE;
		}
	}

	return FALSE;
}

TrueNtCreateFile OriginalNtCreateFile = (TrueNtCreateFile)GetFunctionPtr("ntdll", "NtCreateFile");
TrueNtOpenFile OriginalNtOpenFile = (TrueNtOpenFile)GetFunctionPtr("ntdll", "NtOpenFile");
TrueNtReadFile OriginalNtReadFile = (TrueNtReadFile)GetFunctionPtr("ntdll", "NtReadFile");
TrueNtWriteFile OriginalNtWriteFile = (TrueNtWriteFile)GetFunctionPtr("ntdll", "NtWriteFile");
TrueNtDeleteFile OriginalNtDeleteFile = (TrueNtDeleteFile)GetFunctionPtr("ntdll", "NtDeleteFile");

NTSTATUS WINAPI HookNtCreateFile(
	PHANDLE            FileHandle,
	ACCESS_MASK        DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK   IoStatusBlock,
	PLARGE_INTEGER     AllocationSize,
	ULONG              FileAttributes,
	ULONG              ShareAccess,
	ULONG              CreateDisposition,
	ULONG              CreateOptions,
	PVOID              EaBuffer,
	ULONG              EaLength
) {
	if (path_is_allowed(ConvertLPCWSTRToString(ObjectAttributes->ObjectName->Buffer))) {
		return OriginalNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
	}

	LOG_WSTR(L"CreateFile: %ls -- %x", ObjectAttributes->ObjectName->Buffer, *FileHandle);
	NTSTATUS status =  OriginalNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
	
	ObjectFile* object = new ObjectFile(ObjectAttributes->ObjectName->Buffer);
	mObjectsManager->insertEntry(*FileHandle, object);
	
	// LOG_BYTES(reinterpret_cast<PBYTE>(ObjectAttributes->ObjectName->Buffer), ObjectAttributes->ObjectName->Length);
	
	return status;
}

NTSTATUS NTAPI HookNtOpenFile(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions
) {
	if (path_is_allowed(ConvertLPCWSTRToString(ObjectAttributes->ObjectName->Buffer))) {
		return OriginalNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
	}

	LOG_WSTR(L"OpenFile: %ls", ObjectAttributes->ObjectName->Buffer);
	ObjectFile* ob = new ObjectFile(ObjectAttributes->ObjectName->Buffer);
	mObjectsManager->insertEntry(*FileHandle, ob);

	return OriginalNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}

NTSTATUS NTAPI HookNtReadFile(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
) {
	return OriginalNtReadFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
}

NTSTATUS NTAPI HookNtWriteFile(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
) {
	/*if (mObjectsManager->isExist(FileHandle)) {
		LOG_STR("okie found the handle");
	}*/
	// check FileHandle is valid


	mObjectsManager->initObjectFileBuffer(FileHandle, Buffer, Length);
	ObjectFile* ob = (ObjectFile*)(mObjectsManager->getObject(FileHandle));
	if (ob == NULL) {
		LOG_CSTR("WriteFile: error when get object file");
	}
	/*else {
		LOG_BYTES(reinterpret_cast<PBYTE>(ob->getBuffer()), ob->getLenght());
	}*/
	// LOG_BYTES(reinterpret_cast<PBYTE>(ob->getBuffer()), ob->getLenght());

	return OriginalNtWriteFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
}

NTSTATUS NTAPI HookNtDeleteFile(
	POBJECT_ATTRIBUTES ObjectAttributes
) {
	return OriginalNtDeleteFile(ObjectAttributes);
}

HANDLE HookCreateFileA(
	LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) {
	// MessageBox(GetActiveWindow(), lpFileName, "CreateFileA", MB_OK);
	return TrueCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE HookCreateFileW(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) {
	// MessageBox(GetActiveWindow(), (LPCSTR)lpFileName, "CreateFileW", MB_OK);
	return TrueCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL HookReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
) {
	return TRUE;
}

BOOL HookWriteFile(
	HANDLE       hFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
) {
	return TRUE;
}

BOOL HookDeleteFileW(
	LPCWSTR lpFileName
) {
	return TRUE;
}