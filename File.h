#pragma once

#ifndef FILE_H
#define FILE_H

#include <Windows.h>
#include <iostream>
#include <winternl.h>
#include <winnt.h>
#include <filesystem>
#include <ShlObj.h>

#include "HookFunctions.h"
#include "ObjectsManager.h"
#include "Logs.h"
#include "Utils.h"
#include "Scanner.h"

namespace fs = std::filesystem;

#define DIR_COMMON_APPDATA            0x0023        // All Users\Application Data
#define DIR_WINDOWS                   0x0024        // GetWindowsDirectory()
#define DIR_SYSTEM                    0x0025        // GetSystemDirectory()
#define DIR_PROGRAM_FILES             0x0026        // C:\Program Files
#define DIR_MYPICTURES                0x0027        // C:\Program Files\My Pictures
#define DIR_PROFILE                   0x0028        // USERPROFILE
#define DIR_SYSTEMX86                 0x0029        // x86 system directory on RISC
#define DIR_PROGRAM_FILESX86          0x002a        // x86 C:\Program Files on RISC


static int lstDIRID[] = { DIR_WINDOWS, DIR_SYSTEM, DIR_PROGRAM_FILES, DIR_PROGRAM_FILESX86 };
static std::string lstExtentions[] = { "*" };
static std::string myFile[] = { "Logs.txt", "ConsoleHost_history.txt" };


inline 
BOOL is_belong_to(fs::path path) {
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


inline 
BOOL check_extensions(fs::path pathExtension) {

	std::string endOfFile = pathExtension.string();
	if (endOfFile.back() == '*') {
		return TRUE;
	}

	return FALSE;
}

inline 
BOOL check_file(fs::path pathFile) {
	std::string filename = pathFile.filename().string();
	for (const auto& file : myFile) {
		if (file == filename) {
			return TRUE;
		}
	}
	return FALSE;
}

inline
std::wstring get_file_name(std::wstring path) {
	fs::path pathToFile(path);
	if (pathToFile.has_filename()) {
		return pathToFile.filename().wstring();
	}
	return L"";
}

BOOL path_is_allowed(std::string pathName);

inline
std::wstring get_desktop_path() {
	wchar_t env_path[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOP, NULL, 0, env_path))) {
		return std::wstring(env_path);
	}
	return L"";
}

inline
std::wstring create_directory(std::wstring pathToDir) {
	fs::path path(pathToDir);
	if (fs::exists(path)) {
		return path.wstring();
	}
	fs::create_directory(path);
	return path.wstring();
}

static std::wstring path_to_sandbox = create_directory(get_desktop_path() + L"\\Sandbox\\");

inline
void set_object_attributes(POBJECT_ATTRIBUTES object_attributes) {
	std::wstring file = L"\\??\\" + path_to_sandbox + get_file_name(std::wstring(object_attributes->ObjectName->Buffer));
	object_attributes->ObjectName->Buffer = (PWSTR)file.c_str();
	object_attributes->ObjectName->Length = file.size() * 2;
	object_attributes->ObjectName->MaximumLength = file.size() * 2 + 2;
}

/// <summary>
///  Hooking Nt Functions in ntdll.dll
/// </summary>

static LPCSTR lpHookFiles[] = {"DeleteFileA" "DeleteFileW"};
#define NUM_HOOK_FILES sizeof(lpHookFiles) / sizeof(LPCSTR);

typedef NTSTATUS(NTAPI* TrueNtCreateFile)
(
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
);

typedef NTSTATUS(NTAPI* TrueNtOpenFile) (
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions
);

typedef NTSTATUS(NTAPI* TrueNtReadFile)(
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
);

typedef NTSTATUS(NTAPI* TrueNtWriteFile) (
	HANDLE           FileHandle,
	HANDLE           Event,
	PIO_APC_ROUTINE  ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	PVOID            Buffer,
	ULONG            Length,
	PLARGE_INTEGER   ByteOffset,
	PULONG           Key
);

typedef NTSTATUS(NTAPI* TrueNtDeleteFile) (
	POBJECT_ATTRIBUTES ObjectAttributes
);

extern TrueNtCreateFile OriginalNtCreateFile;
extern TrueNtOpenFile OriginalNtOpenFile; 
extern TrueNtReadFile OriginalNtReadFile;
extern TrueNtWriteFile OriginalNtWriteFile;
extern TrueNtDeleteFile OriginalNtDeleteFile;

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
);

NTSTATUS NTAPI HookNtOpenFile(
	PHANDLE FileHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG ShareAccess,
	ULONG OpenOptions
);

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
);

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
);

NTSTATUS NTAPI HookNtDeleteFile(
	POBJECT_ATTRIBUTES ObjectAttributes
);

ULONG NTAPI HookRtlNtStatusToDosError(
	NTSTATUS Status
);

static HANDLE(*TrueCreateFileA) (
	LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) = CreateFileA;

static HANDLE(*TrueCreateFileW) (
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) = CreateFileW;

static BOOL(*TrueReadFile) (
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
) = ReadFile;

static BOOL(*TrueWriteFile) (
	HANDLE       hFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
) = WriteFile;

typedef BOOL(*TrueDeleteFileW) (
	LPCWSTR lpFileName
);

typedef BOOL(*TrueDeleteFileA) (
	LPCSTR lpFileName
	);

extern TrueDeleteFileW OriginalDeleteFileW;
extern TrueDeleteFileA OriginalDeleteFileA;

HANDLE HookCreateFileA(
	LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
);

HANDLE HookCreateFileW(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
);

BOOL HookReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
);

BOOL HookWriteFile(
	HANDLE       hFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
);

BOOL HookDeleteFileW(
	LPCWSTR lpFileName
);

BOOL HookDeleteFileA(
	LPCSTR lpFileName
);

#endif // !FILE_H
