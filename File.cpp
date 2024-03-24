#pragma once

#include "pch.h"
#include "File.h"

BOOL path_is_allowed(LPCWSTR pathName) {
	
	fs::path path(pathName);
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

TrueNtCreateFile OriginalNtCreateFile = (TrueNtCreateFile)GetFunctionPtr("ntdll", "NtCreateFile");
TrueNtOpenFile OriginalNtOpenFile = (TrueNtOpenFile)GetFunctionPtr("ntdll", "NtOpenFile");
TrueNtReadFile OriginalNtReadFile = (TrueNtReadFile)GetFunctionPtr("ntdll", "NtReadFile");
TrueNtWriteFile OriginalNtWriteFile = (TrueNtWriteFile)GetFunctionPtr("ntdll", "NtWriteFile");
TrueNtDeleteFile OriginalNtDeleteFile = (TrueNtDeleteFile)GetFunctionPtr("ntdll", "NtDeleteFile");

TrueDeleteFileW OriginalDeleteFileW = DeleteFileW;
TrueDeleteFileA OriginalDeleteFileA = DeleteFileA;

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
	std::wstring old_file_name = std::wstring(ObjectAttributes->ObjectName->Buffer);
	// set path of file to Desktop\Sandbox Folder
	set_object_attributes(ObjectAttributes);

	NTSTATUS status =  OriginalNtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
	
	if (*FileHandle != NULL) {
		mLogs->write(L"CreateFile: \n\tFileName: %s", old_file_name.c_str());
	}

	mObjectsManager->insertEntry(*FileHandle, new ObjectFile(old_file_name.c_str()));

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

	std::wstring old_file_name = std::wstring(ObjectAttributes->ObjectName->Buffer);
	set_object_attributes(ObjectAttributes);
	NTSTATUS status = OriginalNtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
	
	if (*FileHandle != NULL) {
		mLogs->write(L"OpenFile: %s", old_file_name.c_str());
	}
	else {
		mLogs->write("%s: FileHandle is NULL", __FUNCTION__); 
	}

	mObjectsManager->insertEntry(*FileHandle, new ObjectFile(old_file_name.c_str()));

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
	if (!mObjectsManager->isExist(FileHandle)) {
		mLogs->write("%s: FileHanle %x is not in ObjectManager", __FUNCTION__, FileHandle);
		goto exit;
	}
	else {
		auto ob = (PObjectFile)(mObjectsManager->getObject(FileHandle));
		mLogs->write(L"\nReadFile: \n\tFileName: %s\n\tLength: %d\n", ob->getFileName().c_str(), Length);
		mLogs->write_dump((PBYTE)Buffer, Length);
	}

	if (scan_memory((PBYTE)Buffer, Length)) {
		mLogs->write("okie scan");
		return NTSTATUS(0);
	}
	else {
		mLogs->write("%s: memory is safety", __FUNCTION__);
	}

exit:
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
	if (!mObjectsManager->isExist(FileHandle)) {
		mLogs->write("%s: FileHanle %x is not in ObjectManager", __FUNCTION__, FileHandle);
		goto exit;
	} 
	else {
		auto ob = (PObjectFile)(mObjectsManager->getObject(FileHandle));
		mLogs->write(L"\nWriteFile: \n\tFileName: %s\n\tLength: %d\n", ob->getFileName().c_str(), Length);
		mLogs->write_dump((PBYTE)Buffer, Length);
	}

	if (scan_memory((PBYTE)Buffer, Length)) {
		// mLogs->write("okie scan");
		return NTSTATUS(0);
	}
	else {
		mLogs->write("%s: memory is safety", __FUNCTION__);
	}

exit:
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
	if (!path_is_allowed(lpFileName)) {
		mLogs->write(L"DeleteFile: %s", lpFileName);
		return TRUE;
	}
	return OriginalDeleteFileW(lpFileName);
}

BOOL HookDeleteFileA(
	LPCSTR lpFileName
) {
	mLogs->write("%s: %s", __FUNCTION__, lpFileName);
	return OriginalDeleteFileA(lpFileName);
}