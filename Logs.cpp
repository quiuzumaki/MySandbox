#include "pch.h"
#include "Logs.h"

//Logs::Logs() {
//	this->mRecordsTable = new RecordsTable();
//}
//
//Logs::~Logs() {
//	delete this->mRecordsTable;
//}
//
//inline BOOL Logs::isExist(LPCSTR lpAPIName) {
//	if (this->isEmpty()) {
//		return FALSE;
//	}
//
//	if (this->mRecordsTable->find(lpAPIName) != this->mRecordsTable->end()) {
//		return TRUE;
//	}
//	return FALSE;
//}
//
//inline BOOL Logs::insertRecord(LPCSTR lpAPIName, Object* object) {
//	if (object == NULL) {
//		return FALSE;
//	}
//
//	(*this->mRecordsTable)[lpAPIName].push_back(object->getInfo());
//	return TRUE;
//}
//
//inline BOOL Logs::insertRecord(LPCSTR lpAPIName, LPCSTR lpInfo) {
//	(*this->mRecordsTable)[lpAPIName].push_back(lpInfo);
//	return TRUE;
//}
//
//VOID Logs::getLogs() {
//	for (RecordsTable::iterator it = this->mRecordsTable->begin(); it != this->mRecordsTable->end(); it++) {
//		std::cout << it->first;
//
//		std::vector<LPCSTR> lstFileName = it->second;
//		std::cout << ": " + std::to_string(lstFileName.size()) + "\n";
//		
//		for (int i = 0; i < lstFileName.size(); i++) {
//			std::cout << "\t\t" + (std::string)(lstFileName[i]) + "\n";
//		}
//		std::cout << "\n";
//	}
//}
//
//inline BOOL Logs::isEmpty() {
//	return this->mRecordsTable->empty();
//}
//
//inline VOID Logs::writeLogsToFile() {
//	// open file 
//	std::ofstream file(pathToFileName);
//
//	if (file.is_open()) {
//		for (RecordsTable::iterator it = this->mRecordsTable->begin(); it != this->mRecordsTable->end(); it++) {
//			file << "==============  " << it->first << "  ==============\n";
//
//			for (int i = 0; i < it->second.size(); i++) {
//				file << "\t\t" << it->second[i] << "\n";
//			}
//		}
//	} 
//
//	file.close();
//}
