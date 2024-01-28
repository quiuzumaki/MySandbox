#pragma once


#ifndef REGISTRY_H
#define REGISTRY_H

#include <Windows.h>

//#define HKEY_CLASSES_ROOT                   (( HKEY ) (ULONG_PTR)((LONG)0x80000000) )
//#define HKEY_CURRENT_USER                   (( HKEY ) (ULONG_PTR)((LONG)0x80000001) )
//#define HKEY_LOCAL_MACHINE                  (( HKEY ) (ULONG_PTR)((LONG)0x80000002) )
//#define HKEY_USERS                          (( HKEY ) (ULONG_PTR)((LONG)0x80000003) )
//#define HKEY_PERFORMANCE_DATA               (( HKEY ) (ULONG_PTR)((LONG)0x80000004) )
//#define HKEY_CURRENT_CONFIG                 (( HKEY ) (ULONG_PTR)((LONG)0x80000005) )


LSTATUS HookRegLoadKeyA(
     HKEY hKey,
     LPCSTR lpSubKey,
     LPCSTR lpFile
);

LSTATUS HookRegLoadKeyW(
     HKEY hKey,
     LPCWSTR lpSubKey,
     LPCWSTR lpFile
);

LSTATUS HookRegOpenKeyA(
     HKEY hKey,
     LPCSTR lpSubKey,
     PHKEY phkResult
);

LSTATUS HookRegOpenKeyW(
     HKEY hKey,
     LPCWSTR lpSubKey,
     PHKEY phkResult
);

LSTATUS HookRegOpenKeyExA(
     HKEY hKey,
     LPCSTR lpSubKey,
     DWORD ulOptions,
     REGSAM samDesired,
     PHKEY phkResult
);

LSTATUS HookRegOpenKeyExW(
     HKEY hKey,
     LPCWSTR lpSubKey,
     DWORD ulOptions,
     REGSAM samDesired,
     PHKEY phkResult
);

#endif // !REGISTRY_H
