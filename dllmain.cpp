// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <fstream>

#include "Logs.h"
#include "HookFunctions.h"

VOID Main() {

    CHAR moduleName[128] = "";
    GetModuleFileNameA(NULL, moduleName, sizeof(moduleName));
    InstallDetoursHook();

    mLogs->open();

    if (mLogs->is_open()) {
        MessageBoxA(NULL, moduleName, "DLL_PROCESS_ATTACH", NULL);
    }
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    if (DetourIsHelperProcess()) {
        return TRUE;
    }
    
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            Main();
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            UninstallDetoursHook();
            break;
    }
    return TRUE;
}