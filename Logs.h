#pragma once

#include <Windows.h>
#include <map>
#include <vector>
#include "ObjectsManager.h"
#include <iostream>
#include <string>

#ifndef LOGS_H
#define LOGS_H

typedef std::pair<LPCSTR, std::vector<LPCSTR>> RecordPair;
typedef std::map<LPCSTR, std::vector<LPCSTR>> RecordsTable;

class Logs {
private:
	RecordsTable* mRecordsTable;
	BOOL isExist(LPCSTR);
public: 
	Logs();
	~Logs();
	BOOL insertRecord(LPCSTR, Object*);
	VOID getLogs();
	BOOL isEmpty();
};

extern Logs* SandboxLogs;

#endif // !LOGS_H
