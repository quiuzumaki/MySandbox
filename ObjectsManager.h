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
public:
	ObjectFile(LPCWSTR filename) {
		this->filename = std::wstring(filename);
	}
	ObjectFile(ObjectFile* object) {
		this->filename = object->filename;
	}
	ObjectFile() {

	}
	~ObjectFile() {
	}

	std::wstring getFileName();
	LPCSTR getInfo();
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
	size_t getSize();
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
size_t ObjectsManager::getSize() {
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
