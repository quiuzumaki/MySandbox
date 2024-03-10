#pragma once

#include <Windows.h>
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include "Utils.h"


#ifndef LOGS_H
#define LOGS_H

//typedef std::pair<LPCSTR, std::vector<LPCSTR>> RecordPair;
//typedef std::map<LPCSTR, std::vector<LPCSTR>> RecordsTable;
//
//class Logs {
//private:
//	RecordsTable* mRecordsTable;
//	BOOL isExist(LPCSTR);
//public: 
//	Logs();
//	~Logs();
//	BOOL insertRecord(LPCSTR, Object*);
//	BOOL insertRecord(LPCSTR, LPCSTR);
//	VOID getLogs();
//	BOOL isEmpty();
//	VOID writeLogsToFile();
//};

// static Logs* SandboxLogs = new Logs();

static LPCSTR pathToFileName = "Logs.txt";
static std::ofstream global_file(pathToFileName);

#define TYPE(id) typeid(id).name();

#define LOG_CWSTR(content) global_file << ConvertLPCWSTRToString(content) << "\n"; 
#define LOG_CSTR(content) global_file << std::string(content) << "\n";

#define LOG_STR(format, ...) \
	char log[1000];\
	sprintf_s(log, sizeof(log), format, __VA_ARGS__); \
	global_file << log << "\n";

#define LOG_WSTR(format, ...) \
	wchar_t log[1000]; \
	swprintf_s(log, sizeof(log), format, __VA_ARGS__); \
	global_file << ConvertLPCWSTRToString(std::wstring(log).c_str()) << "\n";

#define LOG_BYTES(bytes, size) \
	for(int i = 0 ; i < (int)(size / 16); i++) {\
		for(int j = 0; j < 16; j++) {\
			global_file << std::hex << (int)(bytes[16 * i + j]) <<  " "; \
		} \
		global_file << "        "; \
		for(int j = 0; j < 16; j++) {\
			global_file << bytes[16 * i + j]; \
		} global_file << "\n";\
	}
	
#endif // !LOGS_H