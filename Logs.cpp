#include "pch.h"
#include "Logs.h"

Logs::Logs() {
	// this->filename = filename;
	// this->stream.open(this->filename, std::ofstream::binary);
}

Logs::~Logs() {
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

void Logs::write_dump(PBYTE buffer, ULONG length) {
	std::ostringstream stream_buffer;
	const int bytesPerLine = 16;
	for (ULONG i = 0; i < length; i += bytesPerLine) {
		for (int j = 0; j < bytesPerLine; j++) {
			if ((i + j) < length) {
				stream_buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buffer[i + j] << " ";
			}
			else {
				stream_buffer << "   "; // Padding for incomplete lines
			}
		}
		stream_buffer << "     ";
		for (int j = 0; (j < bytesPerLine) && (i + j < length); j++) {
			char c = buffer[i + j];
			stream_buffer << (isprint(c) ? c : '.');
		}
		stream_buffer << "\n";
	}
	this->stream << stream_buffer.str();
}

Logs* mLogs = new Logs();
