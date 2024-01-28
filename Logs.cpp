#include "pch.h"
#include "Logs.h"

Logs::Logs() {
	this->mRecordsTable = new RecordsTable();
}

Logs::~Logs() {
	delete this->mRecordsTable;
}

BOOL Logs::isExist(LPCSTR APIName) {
	if (this->isEmpty()) {
		return FALSE;
	}

	if (this->mRecordsTable->find(APIName) != this->mRecordsTable->end()) {
		return TRUE;
	}
	return FALSE;
}

BOOL Logs::insertRecord(LPCSTR APIName, Object* object) {
	if (object == NULL) {
		return FALSE;
	}

	(*this->mRecordsTable)[APIName].push_back(object->getInfo());
	return TRUE;
}

VOID Logs::getLogs() {
	
	for (RecordsTable::iterator it = this->mRecordsTable->begin(); it != this->mRecordsTable->end(); it++) {
		std::cout << it->first;

		std::vector<LPCSTR> lstFileName = it->second;
		std::cout << ": " + std::to_string(lstFileName.size()) + "\n";
		
		for (int i = 0; i < lstFileName.size(); i++) {
			std::cout << "\t\t" + (std::string)(lstFileName[i]) + "\n";
		}
		std::cout << "\n";
	}
}

BOOL Logs::isEmpty() {
	return this->mRecordsTable->empty();
}

Logs* SandboxLogs = new Logs();
