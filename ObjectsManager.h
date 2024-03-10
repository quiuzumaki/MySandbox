#pragma once

#include <Windows.h>
#include <map>
#include <typeinfo>
#include <iostream>

#include "Logs.h"
#include "Utils.h"

#ifndef OBJECTSMANAGER_H
#define OBJECTSMANAGER_H

#define MAX_SIZE 1000


class Object {
public:
	virtual LPCSTR getInfo() = 0;
};

class ObjectFile : public Object {
private:
	LPCWSTR filename;
	PVOID lpBuffer;
	ULONG length;
public:
	// ObjectFile(LPCSTR, PVOID, ULONG);
	ObjectFile(LPCWSTR filename, PVOID pBuffer = NULL, ULONG length = MAX_SIZE) {
		this->filename = filename;
		this->length = length;
		this->lpBuffer = (PBYTE)malloc(length);

		if (pBuffer != NULL) {
			memcpy(this->lpBuffer, pBuffer, length);
		}
	}

	LPCSTR getFileName();
	LPCSTR getInfo();
	VOID setBuffer(PVOID);
	PVOID getBuffer();
	VOID setLength(ULONG);
	ULONG getLenght();
};

class ObjectRegistry : public Object {
private:
	LPCSTR subKeyName;
	LPCSTR keyName;
public:
	ObjectRegistry(LPCSTR, LPCSTR);
	ObjectRegistry();
	LPCSTR getInfo();
};

typedef std::pair<HANDLE, Object*> HandleEntry;
typedef std::map<HANDLE, Object*> HandleTable;

class ObjectsManager {
private:
	HandleTable* mHandleTable;
public:
	ObjectsManager();
	~ObjectsManager();
	BOOL isExist(HANDLE);
	Object* getObject(HANDLE);
	BOOL insertEntry(HANDLE, Object*);
	BOOL initObjectFileBuffer(HANDLE, PVOID, ULONG);
	BOOL insertEntry(HANDLE);
	BOOL deleteObject(HANDLE);
	DWORD getSize();
	BOOL isEmpty();
};

extern ObjectsManager* mObjectsManager;

#endif // !OBJECTSMANAGER_H
