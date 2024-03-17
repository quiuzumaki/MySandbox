#pragma once

#include <Windows.h>
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "Utils.h"

#ifndef LOGS_H
#define LOGS_H

class Logs {
private:
	std::string filename = "Logs.txt";
	std::ofstream stream;
public:
	Logs();
	~Logs();
	void open();
	BOOL is_open();
	void write(std::string buffer);
	void write(std::wstring buffer);
	void write(LPCSTR format, ...);
	void write(LPCWSTR format, ...);
	void write_dump(PBYTE buffer, ULONG length);
};

inline
void Logs::open() {
	this->stream.open(this->filename);
}

inline
BOOL Logs::is_open() {
	return this->stream.is_open();
}

inline
void Logs::write(std::string buffer) {
	this->stream << buffer << "\n";
}

inline
void Logs::write(std::wstring buffer) {
	this->stream << ConvertLPCWSTRToString(buffer.c_str()) << "\n";
}

extern Logs* mLogs;

void init_logs();

#endif // !LOGS_H