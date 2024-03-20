#pragma once

#include <yara.h>
#include <iostream>

#ifndef SCANNER_H
#define SCANNER_H

#include "Logs.h"

struct USER_DATA_CTX {
	bool match = 0;
	YR_META* meta;
};

int my_callback(
	YR_SCAN_CONTEXT* context,
	int message,
	void* message_data,
	void* user_data
);

void _compiler_callback(
	int error_level,
	const char* file_name,
	int line_number,
	const YR_RULE* rule,
	const char* message,
	void* user_data
);

int compile_rule_ex(char* string, YR_RULES** rules, bool strict_escape_flag);

int compile_rule(char* string, YR_RULES** rules);

bool scan_memory_ex(char* rule, uint8_t* buffer, int size);

bool scan_memory(PBYTE buffer, int size);

#endif // !SCANNER_H
