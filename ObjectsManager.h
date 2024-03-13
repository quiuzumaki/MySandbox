#pragma once

#include <Windows.h>
#include <map>
#include <typeinfo>
#include <iostream>

#include "Logs.h"
#include "Utils.h"

#ifndef OBJECTSMANAGER_H
#define OBJECTSMANAGER_H

#define MAX_SIZE 100

class Object {
public:
	virtual LPCSTR getInfo() {
		return "Object";
	}
};

class ObjectRegistry : public Object {
private:
	LPCSTR subKeyName;
	LPCSTR keyName;
public:
	ObjectRegistry(LPCSTR, LPCSTR);
	ObjectRegistry();
	~ObjectRegistry();
	LPCSTR getInfo();
};

class ObjectFile : public Object {
private:
	LPCWSTR filename;
	PVOID lpBuffer;
	ULONG length;
public:
	ObjectFile(LPCWSTR filename, PVOID pBuffer = NULL, ULONG length = 100) {
		this->filename = filename;
		this->length = 100;
		this->lpBuffer = (PVOID)malloc(this->length);

		if (pBuffer != NULL) {
			memcpy(this->lpBuffer, pBuffer, this->length);
		}
		else {
			memset(this->lpBuffer, 0, this->length);
		}
	}
	ObjectFile() {

	}
	~ObjectFile() {
		delete lpBuffer;
	}

	LPCSTR getFileName();
	LPCSTR getInfo();
	PVOID getBuffer();
	ULONG getLength();
	VOID setBuffer(PVOID);
	VOID setLength(ULONG);
};

typedef std::pair<HANDLE, Object*> HandleEntry;
typedef std::map<HANDLE, Object*> HandleTable;

class ObjectsManager {
private:
	HandleTable* mHandleTable;
public:
	ObjectsManager();
	~ObjectsManager();
	BOOL isExist(const HANDLE);
	Object* getObject(const HANDLE);
	PBYTE getObjectBuffer(HANDLE);
	ULONG getObjectLength(HANDLE);
	BOOL insertEntry(const HANDLE, Object*);
	LPCSTR getObjectType(const HANDLE);
	BOOL deleteObject(HANDLE);
	DWORD getSize();
	BOOL isEmpty();
};

extern ObjectsManager* mObjectsManager;

#endif // !OBJECTSMANAGER_H
