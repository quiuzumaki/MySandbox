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
	ObjectRegistry(ObjectRegistry* object) {
		this->subKeyName = object->subKeyName;
		this->keyName = object->keyName;
	}
	ObjectRegistry();
	~ObjectRegistry();
	LPCSTR getInfo();
};

typedef ObjectRegistry* PObjectRegistry;

class ObjectFile : public Object {
private:
	std::wstring filename;
	PBYTE lpBuffer;
	ULONG length;
public:
	ObjectFile(LPCWSTR filename, PVOID pBuffer = NULL, ULONG length = MAX_SIZE) {
		this->filename = std::wstring(filename);
		this->length = length;
		this->lpBuffer = new BYTE[this->length];

		if (pBuffer != NULL) {
			memcpy(this->lpBuffer, pBuffer, this->length);
		}
		else {
			memset(this->lpBuffer, 0, this->length);
		}
	}
	ObjectFile(ObjectFile* object) {
		this->filename = object->filename;
		this->length = object->length;
		this->lpBuffer = new BYTE[this->length];
		if (object->lpBuffer != NULL) {
			memcpy(this->lpBuffer, object->lpBuffer, this->length);
		}
	}
	ObjectFile() {

	}
	~ObjectFile() {
		delete lpBuffer;
	}

	std::wstring getFileName();
	LPCSTR getInfo();
	PVOID getBuffer();
	ULONG getLength();
	VOID setBuffer(PVOID);
	VOID setLength(ULONG);
};

typedef ObjectFile* PObjectFile;

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
	BOOL insertEntry(const HANDLE, ObjectFile*);
	LPCSTR getObjectType(const HANDLE);
	BOOL deleteObject(HANDLE);
	DWORD getSize();
	BOOL isEmpty();
};

inline
LPCSTR ObjectRegistry::getInfo() {
	return "ObjectRegistry";
}

inline
std::wstring ObjectFile::getFileName() {
	return this->filename;
}

inline
LPCSTR ObjectFile::getInfo() {
	return "ObjectFile";
}

inline
ULONG ObjectFile::getLength() {
	return this->length;
}

inline
VOID ObjectFile::setBuffer(PVOID pBuffer) {
	this->lpBuffer = new BYTE[this->length];
	if (lpBuffer != NULL) {
		memcpy(this->lpBuffer, pBuffer, this->length);
	} else {
		memset(this->lpBuffer, 0, this->length);
	}
}

inline
VOID ObjectFile::setLength(ULONG length = MAX_SIZE) {
	this->length = length;
}

inline 
DWORD ObjectsManager::getSize() {
	return this->mHandleTable->size();
}

inline 
BOOL ObjectsManager::isEmpty() {
	return this->mHandleTable->empty();
}

inline
BOOL ObjectsManager::isExist(const HANDLE handle) {
	if (this->isEmpty()) {
		return FALSE;
	}
	if (this->mHandleTable->find(handle) == this->mHandleTable->end()) {
		return FALSE;
	}
	return TRUE;
}

inline
BOOL ObjectsManager::deleteObject(HANDLE handle) {
	if (this->isEmpty()) {
		return FALSE;
	}
	if (this->isExist(handle)) {
		this->mHandleTable->erase(handle);
		return TRUE;
	}
	return FALSE;
}

extern ObjectsManager* mObjectsManager;

#endif // !OBJECTSMANAGER_H
