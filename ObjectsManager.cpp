#include "pch.h"
#include "ObjectsManager.h"

//ObjectFile::ObjectFile(LPCWSTR filename, PVOID buffer = NULL, ULONG length = MAX_SIZE) {
//	this->filename = filename;
//	this->length = length;
//	this->lpBuffer = (PBYTE)malloc(length);
//
//	if (buffer != NULL) {
//		memcpy(this->lpBuffer, buffer, length);
//	}
//}


LPCSTR ObjectFile::getInfo() {
	return ConvertLPCWSTRToString(this->filename).c_str();
}

VOID ObjectFile::setBuffer(PVOID pBuffer) {
	this->lpBuffer = (PBYTE)malloc(this->length);
	memcpy(this->lpBuffer, pBuffer, this->length);
}

VOID ObjectFile::setLength(ULONG length) {
	this->length = length;
}

PVOID ObjectFile::getBuffer() {
	return this->lpBuffer;
}

ULONG ObjectFile::getLenght() {
	return this->length;
}

ObjectRegistry::ObjectRegistry(LPCSTR keyName, LPCSTR subKeyName): keyName(keyName), subKeyName(subKeyName) {}

inline LPCSTR ObjectRegistry::getInfo() {
	return 0;
}

ObjectsManager::ObjectsManager() {
	this->mHandleTable = new HandleTable();
}

ObjectsManager::~ObjectsManager() {
	delete this->mHandleTable;
}

inline BOOL ObjectsManager::isExist(HANDLE handle) {
	if (this->isEmpty()) {
		return FALSE;
	}
	if (this->mHandleTable->find(handle) == this->mHandleTable->end()) {
		return FALSE;
	}
	return TRUE;
}

Object* ObjectsManager::getObject(HANDLE handle) {
	if (this->isEmpty()) {
		return 0;
	}
	/*if (this->isExist(handle)) {
		return (*this->mHandleTable)[handle];
	}*/
	return (*this->mHandleTable)[handle];
}

BOOL ObjectsManager::insertEntry(HANDLE handle, Object* object) {
	if (handle == NULL || object == NULL)
		return FALSE;
	this->mHandleTable->insert(HandleEntry(handle, object));
	return TRUE;
}

BOOL ObjectsManager::insertEntry(HANDLE handle) {
	if (handle == NULL)
		return FALSE;
	this->mHandleTable->insert(HandleEntry(handle, NULL));
	return TRUE;
}

BOOL ObjectsManager::initObjectFileBuffer(HANDLE handle, PVOID buffer, ULONG length) {
	if (handle == NULL || !this->isExist(handle)) 
		return FALSE;

	LOG_STR("get null object");
	/*((ObjectFile*)(*this->mHandleTable)[handle])->setLength(length);
	((ObjectFile*)(*this->mHandleTable)[handle])->setBuffer(buffer);*/

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