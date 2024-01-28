#include "pch.h"
#include "ObjectsManager.h"

ObjectFile::ObjectFile(LPCSTR name): name(name) {}

LPCSTR ObjectFile::getInfo() {
	return this->name;
}

ObjectRegistry::ObjectRegistry(LPCSTR keyName, LPCSTR subKeyName): keyName(keyName), subKeyName(subKeyName) {}

LPCSTR ObjectRegistry::getInfo() {
	return 0;
}

ObjectsManager::ObjectsManager() {
	this->ObjectsTable = new ObjectMap();
}

ObjectsManager::~ObjectsManager() {
	delete this->ObjectsTable;
}

HANDLE ObjectsManager::generateHandle() {
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	HANDLE handle = (HANDLE)(std::rand() & 0xffffff);
	while (isExist(handle) || (handle == 0)) {
		handle = (HANDLE)(std::rand() & 0xffffff);
	}
	return handle;
}

BOOL ObjectsManager::isExist(HANDLE handle) {
	if (this->isEmpty()) {
		return FALSE;
	}
	if (this->ObjectsTable->find(handle) == this->ObjectsTable->end()) {
		return FALSE;
	}
	return TRUE;
}

BOOL ObjectsManager::isEqual(Object* first, Object* second) {
	if (typeid(first) == typeid(second)) {	
		return first->getInfo() == second->getInfo();
	}
	return FALSE;
}

Object* ObjectsManager::getObject(HANDLE handle) {
	if (this->isEmpty()) {
		return 0;
	}

	if (this->isExist(handle)) {
		auto it = this->ObjectsTable->find(handle);
		return it->second;
	}
	return 0;
}

HANDLE ObjectsManager::insertObject(Object *object) {
	HANDLE handle = this->generateHandle();

	if (handle == 0) 
		return NULL;

	this->ObjectsTable->insert(ObjectPair(handle, object));
	return handle;
}

BOOL ObjectsManager::deleteObject(HANDLE handle) {
	if (this->isEmpty()) {
		return FALSE;
	}
	if (this->isExist(handle)) {
		this->ObjectsTable->erase(handle);
		return TRUE;
	}
	return FALSE;
}

BOOL ObjectsManager::deleteObject(Object* object) {
	for (ObjectMap::iterator it = this->ObjectsTable->begin(); it != this->ObjectsTable->end(); it++) {
		if (this->isEqual(object, it->second)) {
			return this->deleteObject(it->first);
		}
	}
	return FALSE;
}

DWORD ObjectsManager::getSize() {
	return this->ObjectsTable->size();
}

BOOL ObjectsManager::isEmpty() {
	return this->ObjectsTable->empty();
}

ObjectsManager* mObjectsManager = new ObjectsManager();
