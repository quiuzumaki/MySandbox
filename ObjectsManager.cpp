#include "pch.h"
#include "ObjectsManager.h"

ObjectRegistry::ObjectRegistry(std::wstring keyName, std::wstring subKeyName) : keyName(keyName), subKeyName(subKeyName) {}

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

BOOL ObjectsManager::insertEntry(const HANDLE handle, Object* object) {
	if (handle == NULL || object == NULL)
		return FALSE;
	if (this->isExist(handle)) {
		mLogs->write("%s: Can not insert ObjectFile", __FUNCTION__);
		return FALSE;
	}
	if (object->getInfo() == _OBJECT_FILE_) {
		(*this->mHandleTable)[handle] = (PObjectFile)(object);
	}
	else {
		(*this->mHandleTable)[handle] = (PObjectRegistry)(object);
	}
	return TRUE;
}

ObjectsManager* mObjectsManager = new ObjectsManager();