#include "pch.h"
#include "ObjectsManager.h"

ObjectRegistry::ObjectRegistry(LPCSTR keyName, LPCSTR subKeyName) : keyName(keyName), subKeyName(subKeyName) {}

ObjectRegistry::ObjectRegistry() {
	this->keyName = NULL;
	this->subKeyName = NULL;
}

ObjectsManager::ObjectsManager() {
	this->mHandleTable = new HandleTable();
}

ObjectsManager::~ObjectsManager() {
	delete this->mHandleTable;
}

Object* ObjectsManager::getObject(const HANDLE handle) {
	if (this->isEmpty() || !this->isExist(handle)) {
		return NULL;
	}
	return (*this->mHandleTable)[handle];
}

LPCSTR ObjectsManager::getObjectType(const HANDLE handle) {
	if (handle == NULL || !this->isExist(handle)) {
		return NULL;
	}

	return (*this->mHandleTable)[handle]->getInfo();
}

BOOL ObjectsManager::insertEntry(const HANDLE handle, ObjectFile* object) {
	if (handle == NULL || object == NULL)
		return FALSE;
	if (this->isExist(handle)) {
		mLogs->write("%s: Can not insert ObjectFile", __FUNCTION__);
		return FALSE;
	}
	(*this->mHandleTable)[handle] = new ObjectFile(object);
	return TRUE;
}

ObjectsManager* mObjectsManager = new ObjectsManager();