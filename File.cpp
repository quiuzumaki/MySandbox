#pragma once

#include "pch.h"
#include "File.h"

typedef HANDLE (*CreateFile_T)
(
	LPCWSTR               lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
);

CreateFile_T CreateFileOriginal = NULL;

HANDLE HookCreateFileA(
	LPCSTR                lpFileName,
	DWORD                 dwDesiredAccess,
	DWORD                 dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD                 dwCreationDisposition,
	DWORD                 dwFlagsAndAttributes,
	HANDLE                hTemplateFile
) {
	ObjectFile* mFile = new ObjectFile(lpFileName);
	SandboxLogs->insertRecord("CreateFile", mFile);
	return mObjectsManager->insertObject(mFile);
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
	MessageBox(NULL, "Install Hook SuccessFull", "CreateFile", NULL);
	return NULL;
	// return CreateFileOriginal(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL HookWriteFile(
	HANDLE       hFile,
	LPCVOID      lpBuffer,
	DWORD        nNumberOfBytesToWrite,
	LPDWORD      lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
) {
	return SandboxLogs->insertRecord("WriteFile", mObjectsManager->getObject(hFile));
}

BOOL HookWriteFileEx(
	HANDLE                          hFile,
	LPCVOID                         lpBuffer,
	DWORD                           nNumberOfBytesToWrite,
	LPOVERLAPPED                    lpOverlapped,
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
) {
	return SandboxLogs->insertRecord("WriteFileEx", mObjectsManager->getObject(hFile));
}

BOOL HookReadFile(
	HANDLE       hFile,
	LPVOID       lpBuffer,
	DWORD        nNumberOfBytesToRead,
	LPDWORD      lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
) {
	return SandboxLogs->insertRecord("ReadFile", mObjectsManager->getObject(hFile));
}

BOOL HookReadFileEx(
	HANDLE                          hFile,
	LPVOID                          lpBuffer,
	DWORD                           nNumberOfBytesToRead,
	LPOVERLAPPED                    lpOverlapped,
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
) {
	return SandboxLogs->insertRecord("ReadFileEx", mObjectsManager->getObject(hFile));
}

BOOL HookDeleteFileA(
	LPCSTR lpFileName
) {
	ObjectFile* object = new ObjectFile(lpFileName);
	mObjectsManager->deleteObject(object);
	SandboxLogs->insertRecord("DeleteFile", object);
	return TRUE;
}

BOOL HookDeleteFileW(
	LPCWSTR lpFileName
) {
	return TRUE;
}
