#pragma once

#include <yara.h>
#include <iostream>

#ifndef SCANNER_H
#define SCANNER_H

struct USER_DATA_CTX {
	bool match = 0;
	YR_META* meta;
};

class Scanner {
private:
	bool scan_memory_ex(char* rules, uint8_t* buffer, int size);
public:
	Scanner();
	~Scanner();
	int callback(
		YR_SCAN_CONTEXT* context,
		int message,
		void* message_data,
		void* user_data
	);
	bool scan_memory(uint8_t* buffer, int size);
};

#endif // !SCANNER_H
