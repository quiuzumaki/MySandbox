// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include "HookFunctions.h"
#include "Hook.h"
#include <iostream>

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    CHAR moduleName[128] = "";
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            GetModuleFileNameA(NULL, moduleName, sizeof(moduleName));
            InstallDetoursHook();
            if (global_file.is_open())
                MessageBoxA(NULL, moduleName, "DLL_PROCESS_ATTACH", NULL);
            break;
        case DLL_THREAD_ATTACH:
            // MessageBoxA(NULL, "DLL_THREAD_ATTACH", "DLL_THREAD_ATTACH", NULL);
            break;
        case DLL_THREAD_DETACH:
            // MessageBoxA(NULL, "DLL_THREAD_DETACH", "DLL_THREAD_DETACH", NULL);
            break;
        case DLL_PROCESS_DETACH:
            // MessageBoxA(NULL, "DLL_PROCESS_DETACH", "DLL_PROCESS_DETACH", NULL);
            UninstallDetoursHook();
            break;
    }
    return TRUE;
}