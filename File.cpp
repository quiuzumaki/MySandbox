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

BOOL path_is_allowed(LPCWSTR pathName) {
	
	fs::path path(ConvertLPCWSTRToString(pathName));
	if (
		is_belong_to(path)
	) {
		return TRUE;
	}

	if (check_extensions(path) || check_file(path)) {
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
		if(strstr(extension.c_str(), lstExtentions[i].c_str()) != NULL) {
			return TRUE;
		}
	}
	return FALSE;
}

inline BOOL check_file(fs::path pathFile) {
	std::string filename = pathFile.filename().string();
	for (int i = 0; i < sizeof(myFile) / sizeof(std::string); i++) {
		if (!strcmp(filename.c_str(), myFile[i].c_str())) {
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
	if (path_is_allowed(ObjectAttributes->ObjectName->Buffer)) {
		return OriginalNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
	}

	NTSTATUS status =  OriginalNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
	mLogs->write(L"CreateFile: %ls -- %x", ObjectAttributes->ObjectName->Buffer, *FileHandle);
	
	ObjectFile* object = new ObjectFile(ObjectAttributes->ObjectName->Buffer);
	mObjectsManager->insertEntry(*FileHandle, object);
	
	mLogs->write(reinterpret_cast<PBYTE>(ObjectAttributes->ObjectName->Buffer), ObjectAttributes->ObjectName->Length);
	
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
	if (path_is_allowed(ObjectAttributes->ObjectName->Buffer)) {
		return OriginalNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
	}

	mLogs->write(L"OpenFile: %ls", ObjectAttributes->ObjectName->Buffer);
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

	if (Buffer != NULL) {
		char buffer[6] = "hello";
		PCHAR c = (PCHAR)malloc(Length);
		memcpy(c, buffer, 6);
		mLogs->write(std::string(c));
	}

	ObjectFile* ob = (ObjectFile*)(mObjectsManager->getObject(FileHandle));
	
	if (ob == NULL) {
		mLogs->write("WriteFile: error when get object file %x", FileHandle);
	} 
	else {
		mLogs->write("WriteFile: %x", FileHandle);
		
		// mLogs->write("\nObjectFile length: %d\n", ob->getLength());
		// mLogs->write(reinterpret_cast<PBYTE>(ob->getBuffer()), ob->getLength());
	}

	return OriginalNtWriteFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
}

NTSTATUS NTAPI HookNtDeleteFile(
	POBJECT_ATTRIBUTES ObjectAttributes
) {
	mLogs->write(L"DeleteFile: %ls", ObjectAttributes->ObjectName->Buffer);
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