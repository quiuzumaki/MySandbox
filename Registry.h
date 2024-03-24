#pragma once


#ifndef REGISTRY_H
#define REGISTRY_H

#include <Windows.h>
#include <winternl.h>

#include "Logs.h"
#include "ObjectsManager.h"
#include "HookFunctions.h"

//#define HKEY_CLASSES_ROOT                   (( HKEY ) (ULONG_PTR)((LONG)0x80000000) )
//#define HKEY_CURRENT_USER                   (( HKEY ) (ULONG_PTR)((LONG)0x80000001) )
//#define HKEY_LOCAL_MACHINE                  (( HKEY ) (ULONG_PTR)((LONG)0x80000002) )
//#define HKEY_USERS                          (( HKEY ) (ULONG_PTR)((LONG)0x80000003) )
//#define HKEY_PERFORMANCE_DATA               (( HKEY ) (ULONG_PTR)((LONG)0x80000004) )
//#define HKEY_CURRENT_CONFIG                 (( HKEY ) (ULONG_PTR)((LONG)0x80000005) )

typedef enum _KEY_INFORMATION_CLASS
{
    KeyBasicInformation,                    // 0x00
    KeyNodeInformation,                     // 0x01
    KeyFullInformation,                     // 0x02
    KeyNameInformation,                     // 0x03
    KeyCachedInformation,                   // 0x04
    KeyFlagsInformation,                    // 0x05
    MaxKeyInfoClass                         // MaxKeyInfoClass should always be the last enum
} KEY_INFORMATION_CLASS;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,               // 0x00
    KeyValueFullInformation,                // 0x01
    KeyValuePartialInformation,             // 0x02
    KeyValueFullInformationAlign64,         // 0x03
    KeyValuePartialInformationAlign64,      // 0x04
    MaxKeyValueInfoClass                    // MaxKeyValueInfoClass should always be the last enum
} KEY_VALUE_INFORMATION_CLASS;


typedef NTSTATUS(NTAPI* TrueNtCreateKey) (
    PHANDLE            KeyHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    ULONG              TitleIndex,
    PUNICODE_STRING    Class,
    ULONG              CreateOptions,
    PULONG             Disposition
);

typedef NTSTATUS(NTAPI* TrueNtOpenKey) (
    PHANDLE            KeyHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes
);

typedef NTSTATUS(NTAPI *TrueNtSetValueKey) (
    PHANDLE            KeyHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes
);

typedef NTSTATUS(NTAPI* TrueNtQueryKey) (
    HANDLE                KeyHandle,
    KEY_INFORMATION_CLASS KeyInformationClass,
    PVOID                 KeyInformation,
    ULONG                 Length,
    PULONG                ResultLength
);

typedef NTSTATUS(NTAPI* TrueNtQueryValueKey) (
    HANDLE                      KeyHandle,
    PUNICODE_STRING             ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      ResultLength
);

extern TrueNtCreateKey OriginalNtCreateKey;
extern TrueNtOpenKey OriginalNtOpenKey;
extern TrueNtSetValueKey OriginalNtSetValueKey;
extern TrueNtQueryKey OriginalNtQueryKey;
extern TrueNtQueryValueKey OriginalNtQueryValueKey;

NTSTATUS HookNtCreateKey(
    PHANDLE            KeyHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    ULONG              TitleIndex,
    PUNICODE_STRING    Class,
    ULONG              CreateOptions,
    PULONG             Disposition
);

NTSTATUS HookNtOpenKey(
    PHANDLE            KeyHandle,
    ACCESS_MASK        DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes
);

NTSTATUS HookNtSetValueKey(
    HANDLE          KeyHandle,
    PUNICODE_STRING ValueName,
    ULONG           TitleIndex,
    ULONG           Type,
    PVOID           Data,
    ULONG           DataSize
);

NTSTATUS HookNtQueryKey(
    HANDLE                KeyHandle,
    KEY_INFORMATION_CLASS KeyInformationClass,
    PVOID                 KeyInformation,
    ULONG                 Length,
    PULONG                ResultLength
);

NTSTATUS HookNtQueryValueKey(
    HANDLE                      KeyHandle,
    PUNICODE_STRING             ValueName,
    KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    PVOID                       KeyValueInformation,
    ULONG                       Length,
    PULONG                      ResultLength
);

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
