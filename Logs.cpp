#include "pch.h"
#include "Logs.h"

Logs::Logs() {
	// this->filename = filename;
	// this->stream.open(this->filename, std::ofstream::binary);
}

Logs::~Logs() {
	this->stream.close();
}

void Logs::open() {
	this->stream.open(this->filename, std::ofstream::binary);
}

BOOL Logs::is_open() {
	return this->stream.is_open();
}

void Logs::write(std::string buffer) {
	this->stream << buffer << "\n";
}

void Logs::write(std::wstring buffer) {
	this->stream << ConvertLPCWSTRToString(buffer.c_str()) << "\n";
}

void Logs::write(LPCSTR format, ...) {
	char buffer[1000];
	va_list args;
	va_start(args, format);
	sprintf_s(buffer, sizeof(buffer), format, args);
	this->stream << std::string(buffer) << "\n";
	va_end(args);
}

void Logs::write(LPCWSTR format, ...) {
	wchar_t buffer[1000];
	va_list args;
	va_start(args, format);
	vswprintf(buffer, sizeof(buffer) / sizeof(wchar_t), format, args);
	this->stream << ConvertLPCWSTRToString(std::wstring(buffer).c_str()) << "\n";
	va_end(args);
}

void Logs::write(PBYTE buffer, ULONG length) {
	int x = length / 16;
	int y = length % 16;
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < 16; j++) {
			this->stream << std::setfill('0') << std::setw(2) << std::hex << (int)buffer[16*i+j] << " ";
		}
		this->stream << "     ";
		for (int j = 0; j < 16; j++) {
			this->stream << (char)buffer[16 * i + j];
		}
		this->stream << "\n";
	}

	for (int j = 0; j < y; j++) {
		this->stream << std::setfill('0') << std::setw(2) << std::hex << (int)buffer[16 * x + j] << " ";
	}
	// this->stream << std::setfill(' ') << std::setw(37) << std::string(' ');
	for (int j = 0; j < y; j++) {
		this->stream << (char)buffer[16 * x + j];
	}
	this->stream << "\n";
}

Logs* mLogs = new Logs();

void init_logs() {
	mLogs->open();
}
