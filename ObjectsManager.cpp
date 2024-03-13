#include "pch.h"
#include "ObjectsManager.h"

LPCSTR ObjectFile::getInfo() {
	return "ObjectFile";
}

VOID ObjectFile::setBuffer(PVOID pBuffer) {
	this->lpBuffer = (PBYTE)malloc(this->length);
	if (lpBuffer != NULL) {
		memcpy(this->lpBuffer, pBuffer, this->length);
	}
}

VOID ObjectFile::setLength(ULONG length) {
	this->length = length;
}

PVOID ObjectFile::getBuffer() {
	return this->lpBuffer;
}

ULONG ObjectFile::getLength() {
	return this->length;
}

ObjectRegistry::ObjectRegistry(LPCSTR keyName, LPCSTR subKeyName) : keyName(keyName), subKeyName(subKeyName) {}

ObjectRegistry::ObjectRegistry() {
	this->keyName = NULL;
	this->subKeyName = NULL;
}

LPCSTR ObjectRegistry::getInfo() {
	return "ObjectRegistry";
}

ObjectsManager::ObjectsManager() {
	this->mHandleTable = new HandleTable();
}

ObjectsManager::~ObjectsManager() {
	delete this->mHandleTable;
}

inline BOOL ObjectsManager::isExist(const HANDLE handle) {
	if (this->isEmpty()) {
		return FALSE;
	}
	if (this->mHandleTable->find(handle) == this->mHandleTable->end()) {
		return FALSE;
	}
	return TRUE;
}

Object* ObjectsManager::getObject(const HANDLE handle) {
	if (this->isEmpty()) {
		return 0;
	}
	return (*this->mHandleTable)[handle];
}

//PBYTE ObjectsManager::getObjectBuffer(HANDLE handle) {
//
//	if (!strcmp((*this->mHandleTable)[handle]->getInfo(), "ObjectFile")) {
//		return (PBYTE)((ObjectFile*)((*this->mHandleTable)[handle]))->getBuffer();
//	}
//	return NULL;
//}
//ULONG ObjectsManager::getObjectLength(HANDLE handle) {
//	if (!strcmp((*this->mHandleTable)[handle]->getInfo(), "ObjectFile")) {
//		return ((ObjectFile*)((*this->mHandleTable)[handle]))->getLength();
//	}
//	return NULL;
//}

LPCSTR ObjectsManager::getObjectType(const HANDLE handle) {
	if (handle == NULL || !this->isExist(handle)) {
		return NULL;
	}

	return (*this->mHandleTable)[handle]->getInfo();
}

BOOL ObjectsManager::insertEntry(const HANDLE handle, Object* object) {
	if (handle == NULL || object == NULL)
		return FALSE;
	if (!strcmp(object->getInfo(), "ObjectFile")) {
		(*this->mHandleTable)[handle] = new ObjectFile();
		memcpy((*this->mHandleTable)[handle], object, sizeof(ObjectFile));
	}
	else {
		(*this->mHandleTable)[handle] = new ObjectRegistry();
		memcpy((*this->mHandleTable)[handle], object, sizeof(ObjectRegistry));
	}

	return TRUE;
}

inline BOOL ObjectsManager::deleteObject(HANDLE handle) {
	if (this->isEmpty()) {
		return FALSE;
	}
	if (this->isExist(handle)) {
		this->mHandleTable->erase(handle);
		return TRUE;
	}
	return FALSE;
}

inline DWORD ObjectsManager::getSize() {
	return this->mHandleTable->size();
}

inline BOOL ObjectsManager::isEmpty() {
	return this->mHandleTable->empty();
}

ObjectsManager* mObjectsManager = new ObjectsManager();