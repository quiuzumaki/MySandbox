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

#define _OBJECT_			0x0
#define _OBJECT_FILE_		0x1
#define _OBJECT_REGISTRY_	0x2

class Object {
public:
	virtual int getInfo() {
		return _OBJECT_;
	}
};

class ObjectRegistry : public Object {
private:
	std::wstring subKeyName;
	std::wstring keyName;
public:
	ObjectRegistry(std::wstring, std::wstring);
	ObjectRegistry(ObjectRegistry* object) {
		this->subKeyName = object->subKeyName;
		this->keyName = object->keyName;
	}
	ObjectRegistry() {}
	~ObjectRegistry();
	std::wstring getSubKey();
	std::wstring getKey();
	int getInfo();
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
	int getInfo();
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
	BOOL insertEntry(const HANDLE, Object*);
	int getObjectType(const HANDLE);
	BOOL deleteObject(HANDLE);
	BOOL deleteObject(std::wstring);
	size_t getSize();
	BOOL isEmpty();
};

inline
int ObjectRegistry::getInfo() {
	return _OBJECT_REGISTRY_;
}

inline
std::wstring ObjectRegistry::getSubKey() {
	return this->subKeyName;
}

inline
std::wstring ObjectRegistry::getKey() {
	return this->keyName;
}

inline
std::wstring ObjectFile::getFileName() {
	return this->filename;
}

inline
int ObjectFile::getInfo() {
	return _OBJECT_FILE_;
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
int ObjectsManager::getObjectType(const HANDLE handle) {
	if (handle == NULL || !this->isExist(handle)) {
		return NULL;
	}

	return (*this->mHandleTable)[handle]->getInfo();
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

inline
BOOL ObjectsManager::deleteObject(std::wstring lpFileName) {
	if (this->isEmpty()) {
		return FALSE;
	}
	for (auto& pair : *(this->mHandleTable)) {
	}
}

extern ObjectsManager* mObjectsManager;

#endif // !OBJECTSMANAGER_H
