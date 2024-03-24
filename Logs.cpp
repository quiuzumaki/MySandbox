#include "pch.h"
#include "Logs.h"

Logs::Logs() {
	// this->filename = filename;
	// this->stream.open(this->filename, std::ofstream::binary);
}

Logs::~Logs() {
	this->stream.close();
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
