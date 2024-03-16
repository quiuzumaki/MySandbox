#include "pch.h"
#include "Logs.h"

Logs::Logs() {
	// this->filename = filename;
	// this->stream.open(this->filename, std::ofstream::binary);
}

Logs::~Logs() {
	mLogs->write("File cancel");
	this->stream.close();
}

void Logs::write(LPCSTR format, ...) {
	va_list args;
	va_start(args, format);
	int size = _vscprintf(format, args) + 1;
	char* buffer = new char[size];
	vsprintf_s(buffer, size, format, args);
	va_end(args);
	this->stream << std::string(buffer) << "\n";
}

void Logs::write(LPCWSTR format, ...) {
	va_list args;
	va_start(args, format);
	int size = _vscwprintf(format, args) + 1;
	wchar_t* buffer = new wchar_t[size];
	vswprintf_s(buffer, size, format, args);
	va_end(args);
	this->stream << ConvertLPCWSTRToString(std::wstring(buffer).c_str()) << "\n";
}

void Logs::write(PBYTE buffer, ULONG length) {
	const int bytesPerLine = 16;
	for (ULONG i = 0; i < length; i += bytesPerLine) {
		for (int j = 0; j < bytesPerLine; j++) {
			if ((i + j) < length) {
				this->stream << std::setfill('0') << std::setw(2) << std::hex << (int)buffer[i + j] << " ";
			}
			else {
				this->stream << "   "; // Padding for incomplete lines
			}
		}
		this->stream << "     ";
		for (int j = 0; (j < bytesPerLine) && (i + j < length); j++) {
			char c = buffer[i + j];
			this->stream << (isprint(c) ? c : '.');
		}
		this->stream << "\n";
	}
}

//void Logs::dump(LPVOID buffer) {
//	size_t wcharSize = wcslen(wideString) * sizeof(WCHAR);
//
//	// Allocate memory for the byte array
//	PBYTE byteArray = new BYTE[wcharSize];
//
//	// Copy the wide character string to the byte array
//	memcpy(byteArray, reinterpret_cast<PBYTE>(wideString), wcharSize);
//
//	this->write(byteArray, wcharSize);
//}

Logs* mLogs = new Logs();

void init_logs() {
	mLogs->open();
}
