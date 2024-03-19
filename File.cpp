#pragma once

#include "pch.h"
#include "File.h"

inline BOOL is_belong_to(fs::path path) {
	char env_path[MAX_PATH];
	std::string relative_path = toLowercase(path.relative_path().string());

	for (int i = 0; i < sizeof(lstDIRID) / sizeof(int); i++) {
		if (SUCCEEDED(SHGetFolderPathA(NULL, lstDIRID[i], NULL, 0, env_path))) {
			if (relative_path.find(toLowercase(env_path)) != std::string::npos)
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

	std::string endOfFile = pathExtension.string();
	if (endOfFile.back() == '*') {
		return TRUE;
	}

	return FALSE;
}

inline BOOL check_file(fs::path pathFile) {
	std::string filename = pathFile.filename().string();
	for (const auto& file : myFile) {
		if (file == filename) {
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
	
	ObjectFile* object = new ObjectFile(ObjectAttributes->ObjectName->Buffer);
	mObjectsManager->insertEntry(*FileHandle, object);

	if (DELETE & DesiredAccess) {
		mLogs->write("Create--DeleteFile %x", *FileHandle);
		mLogs->write(object->getFileName());
		return status;
	}

	mLogs->write(ObjectAttributes->ObjectName->Buffer);
	
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

	NTSTATUS status = OriginalNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);


	if (DELETE & DesiredAccess) {
		mLogs->write("Open--DeleteFile %x", *FileHandle);
		return status;
	}

	mLogs->write(L"OpenFile: %s", ObjectAttributes->ObjectName->Buffer);
	return status;
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

	ObjectFile* ob = (ObjectFile*)(mObjectsManager->getObject(FileHandle));
	
	if (ob == NULL) {
		// mLogs->write("WriteFile: error when get object file %x", FileHandle);
	} 
	else {
		mLogs->write("WriteFile with Filename: %x \nLength: %d ============ handle: %x", (mObjectsManager->getObject(FileHandle)), Length, FileHandle);
		mLogs->write(ob->getFileName());	
		mLogs->write_dump(reinterpret_cast<PBYTE>(Buffer), 100);
	}

	return OriginalNtWriteFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key);
}

NTSTATUS NTAPI HookNtDeleteFile(
	POBJECT_ATTRIBUTES ObjectAttributes
) {
	mLogs->write(L"DeleteFile: %s", ObjectAttributes->ObjectName->Buffer);
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