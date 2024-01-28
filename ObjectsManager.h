#pragma once

#include <Windows.h>
#include <map>
#include <cstdlib>
#include <ctime>
#include <typeinfo>

#ifndef OBJECTSMANAGER_H
#define OBJECTSMANAGER_H

class Object {
public:
	virtual LPCSTR getInfo() = 0;
};

class ObjectFile : public Object {
private:
	LPCSTR name;
public:
	ObjectFile(LPCSTR);
	LPCSTR getInfo();
};

class ObjectRegistry : public Object {
private:
	LPCSTR subKeyName;
	LPCSTR keyName;
public:
	ObjectRegistry(LPCSTR, LPCSTR);
	LPCSTR getInfo();
};

typedef std::pair<HANDLE, Object*> ObjectPair;
typedef std::map<HANDLE, Object*> ObjectMap;

class ObjectsManager {
private:
	ObjectMap *ObjectsTable;
	HANDLE generateHandle();
	BOOL isExist(HANDLE);
	BOOL isEqual(Object*, Object*);
public:
	ObjectsManager();
	~ObjectsManager();
	Object* getObject(HANDLE);
	HANDLE insertObject(Object*);
	BOOL deleteObject(HANDLE);
	BOOL deleteObject(Object*);
	DWORD getSize();
	BOOL isEmpty();
};

extern ObjectsManager* mObjectsManager;

#endif // !OBJECTSMANAGER_H
