#pragma once

#include <Windows.h>
#include "File.h"
#include <detours.h>

#ifndef HOOKFUNCTIONS_H
#define HOOKFUNCTIONS_H

LPVOID GetFunctionPtr(LPCSTR lpModule, LPCSTR lpAPIName); 

BOOL InstallDetoursHook();
BOOL UninstallDetoursHook();

#endif // !HOOKFUNCTIONS_H
